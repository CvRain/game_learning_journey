//
// Created by cvrain on 2025/12/26.
//
module;
#include <SDL3/SDL.h>
#include <format>
#include <string_view>
#include <vector>

export module Points.Application;

export class Application {
public:
    explicit Application(const std::string_view &title, int width, int height);
    ~Application();
    auto handle_event(const SDL_Event *event) -> SDL_AppResult;
    auto update() -> SDL_AppResult;

private:
    const std::string_view window_title;
    const int window_width;
    const int window_height;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    Uint64 last_time = 0;
    Uint64 last_wind_change = 0;
    const int num_points = 1200; // 增加粒子数量
    const int min_pixel_per_second = 20;
    const int max_pixel_per_second = 120;

    // 风的参数
    float wind_direction_x = 0.0f; // 风的水平方向 (-1.0 到 1.0)
    float wind_speed = 0.0f; // 风速
    float target_wind_direction_x = 0.0f; // 目标风向
    float target_wind_speed = 0.0f; // 目标风速
    const float max_wind_speed = 80.0f;
    const int wind_change_interval = 3000; // 风向改变间隔（毫秒）
    const float wind_transition_speed = 0.5f; // 风向过渡速度

    std::vector<SDL_FPoint> points{};
    std::vector<float> point_speeds{};
    std::vector<float> point_sizes{}; // 雪花大小
    std::vector<Uint8> point_alphas{}; // 雪花透明度
    std::vector<float> point_swing_phase{}; // 摆动相位
    std::vector<float> point_swing_amplitude{}; // 摆动幅度
};

Application::Application(const std::string_view &title, const int width, const int height) :
    window_title{title}, window_width{width}, window_height{height} {
    SDL_SetAppMetadata("Example Renderer Points", "1.0", "com.claude-rainer.renderer-points");

    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("Couldn't initialize SDL {}", SDL_GetError());
        SDL_Log(result.data());
        throw std::runtime_error(result);
    }

    if (not SDL_CreateWindowAndRenderer(title.data(), width, height, 0, &window, &renderer)) {
        const auto result = std::format("Couldn't create window and renderer: {}", SDL_GetError());
        SDL_Log(result.data());
        throw std::runtime_error(result);
    }

    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    points.resize(num_points);
    point_speeds.resize(num_points);
    point_sizes.resize(num_points);
    point_alphas.resize(num_points);
    point_swing_phase.resize(num_points);
    point_swing_amplitude.resize(num_points);

    for (int i = 0; i < num_points; ++i) {
        points.at(i).x = SDL_randf() * static_cast<float>(window_width);
        points.at(i).y = SDL_randf() * static_cast<float>(window_height);
        point_speeds.at(i) = min_pixel_per_second + SDL_randf() * (max_pixel_per_second - min_pixel_per_second);

        // 不同大小的雪花 (1.0 到 4.0)
        point_sizes.at(i) = 1.0f + SDL_randf() * 3.0f;

        // 不同透明度，大的雪花更不透明（更近），小的更透明（更远）
        point_alphas.at(i) = static_cast<Uint8>(150 + (point_sizes.at(i) / 4.0f) * 105);

        // 随机摆动相位
        point_swing_phase.at(i) = SDL_randf() * 6.28f; // 0 到 2π

        // 摆动幅度与大小相关，小雪花摆动更明显
        point_swing_amplitude.at(i) = (5.0f - point_sizes.at(i)) * 8.0f;
    }

    last_time = SDL_GetTicks();
    last_wind_change = last_time;

    // 初始化随机风向
    wind_direction_x = (SDL_randf() * 2.0f - 1.0f); // -1.0 到 1.0
    wind_speed = SDL_randf() * max_wind_speed;
    target_wind_direction_x = wind_direction_x;
    target_wind_speed = wind_speed;
}

Application::~Application() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

auto Application::handle_event(const SDL_Event *event) -> SDL_AppResult {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

auto Application::update() -> SDL_AppResult {
    // a basic window with background color which like night
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);

    const auto now = SDL_GetTicks();
    const auto elapsed = static_cast<float>(now - last_time) / 1000.0f; /* in seconds */

    // 定期设置新的目标风向和风速
    if (now - last_wind_change > wind_change_interval) {
        target_wind_direction_x = (SDL_randf() * 2.0f - 1.0f); // -1.0 到 1.0
        target_wind_speed = SDL_randf() * max_wind_speed;
        last_wind_change = now;

        SDL_Log("Wind target changed: direction=%.2f, speed=%.2f", target_wind_direction_x, target_wind_speed);
    }

    // 平滑过渡到目标风向和风速
    wind_direction_x += (target_wind_direction_x - wind_direction_x) * wind_transition_speed * elapsed;
    wind_speed += (target_wind_speed - wind_speed) * wind_transition_speed * elapsed;

    // 更新每个粒子的位置
    for (int i = 0; i < num_points; ++i) {
        // 垂直方向：主要受重力影响（向下）
        const auto vertical_distance = elapsed * point_speeds.at(i);
        points.at(i).y += vertical_distance;

        // 水平方向：受风影响
        const auto horizontal_distance = elapsed * wind_speed * wind_direction_x;
        points.at(i).x += horizontal_distance;

        // 添加正弦摆动效果
        point_swing_phase.at(i) += elapsed * 2.0f; // 摆动速度
        const auto swing_offset = SDL_sinf(point_swing_phase.at(i)) * point_swing_amplitude.at(i) * elapsed;
        points.at(i).x += swing_offset;

        // 当粒子超出屏幕边界时，重新从顶部随机位置生成
        if (points.at(i).y > window_height || points.at(i).x < -50 || points.at(i).x > window_width + 50) {
            // 从顶部随机位置重新生成
            points.at(i).x = SDL_randf() * static_cast<float>(window_width);
            points.at(i).y = -20.0f; // 从顶部稍微上方开始
            // 随机化下落速度
            point_speeds.at(i) = min_pixel_per_second + SDL_randf() * (max_pixel_per_second - min_pixel_per_second);
            // 重新随机化摆动相位
            point_swing_phase.at(i) = SDL_randf() * 6.28f;
        }

        // 根据大小和透明度绘制雪花
        const auto size = point_sizes.at(i);
        const auto alpha = point_alphas.at(i);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);

        if (size <= 1.5f) {
            // 小雪花：单个点
            SDL_RenderPoint(renderer, points.at(i).x, points.at(i).y);
        }
        else {
            // 大雪花：绘制小圆形
            const auto center_x = points.at(i).x;
            const auto center_y = points.at(i).y;
            const auto radius = static_cast<int>(size);

            // 绘制填充圆（使用简单的点阵方法）
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    if (dx * dx + dy * dy <= radius * radius) {
                        SDL_RenderPoint(renderer, center_x + dx, center_y + dy);
                    }
                }
            }
        }
    }

    last_time = now;

    SDL_RenderPresent(renderer); /* put it all on the screen! */
    return SDL_APP_CONTINUE;
}
