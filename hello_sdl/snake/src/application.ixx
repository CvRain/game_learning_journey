module;
#include <SDL3/SDL.h>
#include <entt/entt.hpp>
#include <format>
#include <string_view>

import snake;
export module snake.application;

export class Application {
public:
    explicit Application(std::string_view window_title = "Snake", int width = 640, int height = 480);
    ~Application();
    auto handle_event(SDL_Event *event) -> SDL_AppResult;
    auto handle_iteration() -> SDL_AppResult;

private:
    auto render(entt::registry &registry, SDL_Renderer *renderer) -> void;

    const std::string_view window_title;
    const int window_width = 640;
    const int window_height = 480;
    const int step_delay_ms = 200; // 每一步的延迟时间，单位为毫秒
    int snake_length = 3; // 蛇的当前长度

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    entt::registry registry;
};

Application::Application(const std::string_view window_title, const int width, const int height) :
    window_title(window_title), window_width(width), window_height(height) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }

    if (not SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                        &renderer)) {
        const auto result = std::format("SDL_CreateWindowAndRenderer Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }
    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    const auto head = registry.create();
    registry.emplace<Position>(head, 10, 10);
    registry.emplace<SnakeSegment>(head, snake_length); // 初始长度为3
    registry.emplace<Direction>(head, 1, 0); // 初始向右移动
    registry.emplace<SnakeHead>(head); // 标记为蛇头

    const auto food = registry.create();
    registry.emplace<Position>(food, 5, 5);
    registry.emplace<Food>(food);
}

Application::~Application() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

auto Application::handle_event(SDL_Event *event) -> SDL_AppResult {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        auto head_view = registry.view<Direction, SnakeHead>();
        for (const auto &entity: head_view) {
            auto &[dx, dy] = head_view.get<Direction>(entity);
            switch (event->key.scancode) {
                case SDL_SCANCODE_UP:
                    if (dy != 1) { // 防止反向移动
                        dx = 0;
                        dy = -1;
                    }
                    break;
                case SDL_SCANCODE_DOWN:
                    if (dy != -1) {
                        dx = 0;
                        dy = 1;
                    }
                    break;
                case SDL_SCANCODE_LEFT:
                    if (dx != 1) {
                        dx = -1;
                        dy = 0;
                    }
                    break;
                case SDL_SCANCODE_RIGHT:
                    if (dx != -1) {
                        dx = 1;
                        dy = 0;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return SDL_APP_CONTINUE;
}

auto Application::handle_iteration() -> SDL_AppResult {
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);

    const auto current_tick = SDL_GetTicks();
    static auto last_step_tick = current_tick;

    if (current_tick - last_step_tick >= step_delay_ms) {
        last_step_tick = current_tick;

        // 1. 提取当前蛇头的信息
        auto head_view = registry.view<Position, Direction, SnakeHead>();
        entt::entity old_head = head_view.front();
        auto [old_pos, head_dir] = head_view.get<Position, Direction>(old_head);

        // 2. 计算新位置
        Position next_pos = {old_pos.x + head_dir.dx, old_pos.y + head_dir.dy};

        // 3. 边界检测
        const int grid_width = window_width / 24;
        const int grid_height = window_height / 24;
        if (next_pos.x < 0 || next_pos.x >= grid_width || next_pos.y < 0 || next_pos.y >= grid_height) {
            SDL_Log("Game Over: Hit the boundary!");
            return SDL_APP_SUCCESS; // 撞墙，游戏结束
        }

        // 4. 自身碰撞检测（排除当前蛇头，因为蛇头位置即将变成蛇身）
        auto body_view = registry.view<Position, SnakeSegment>();
        for (auto entity: body_view) {
            // 跳过当前蛇头，只检测真正的蛇身部分
            if (entity == old_head) {
                continue;
            }
            const auto &body_pos = body_view.get<Position>(entity);
            if (body_pos.x == next_pos.x && body_pos.y == next_pos.y) {
                SDL_Log("Game Over: Hit yourself!");
                return SDL_APP_SUCCESS; // 撞到自己，游戏结束
            }
        }

        // 5. 碰撞检测 (食物)
        bool eating = false;
        auto food_view = registry.view<Position, Food>();
        for (auto f_entity: food_view) {
            if (auto &f_pos = food_view.get<Position>(f_entity); f_pos.x == next_pos.x && f_pos.y == next_pos.y) {
                eating = true;
                snake_length++; // 增加蛇的长度
                // 简单的食物重生逻辑
                f_pos.x = SDL_rand(grid_width);
                f_pos.y = SDL_rand(grid_height);
                break;
            }
        }

        // 6. 处理蛇身逻辑
        // 给所有蛇节 age - 1 (如果没吃到东西)
        auto seg_view = registry.view<SnakeSegment>();
        for (auto entity: seg_view) {
            auto &seg = seg_view.get<SnakeSegment>(entity);
            if (!eating) {
                seg.age--;
            }
            if (seg.age <= 0) {
                registry.destroy(entity);
            }
        }

        // 7. 创建新头（它继承了旧头的方向和当前长度）
        auto new_entity = registry.create();
        registry.emplace<Position>(new_entity, next_pos);
        registry.emplace<Direction>(new_entity, head_dir); // 保持方向
        registry.emplace<SnakeSegment>(new_entity, snake_length); // 使用当前长度
        registry.emplace<SnakeHead>(new_entity); // 标记为蛇头

        // 旧头不再是头了，移除头部相关组件
        registry.remove<Direction>(old_head);
        registry.remove<SnakeHead>(old_head);
    }
    render(registry, renderer);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

auto Application::render(entt::registry &registry, SDL_Renderer *renderer) -> void {
    // 绘制边界
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 白色边界
    SDL_FRect boundary{.x = 0, .y = 0, .w = static_cast<float>(window_width), .h = static_cast<float>(window_height)};
    SDL_RenderRect(renderer, &boundary);

    // 绘制游戏实体
    const auto view = registry.view<Position>();
    for (const auto &entity: view) {
        const auto &position = view.get<Position>(entity);

        if (registry.all_of<Food>(entity)) {
            // 食物：红色
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        else if (registry.all_of<SnakeHead>(entity)) {
            // 蛇头：亮绿色/黄绿色
            SDL_SetRenderDrawColor(renderer, 150, 255, 0, 255);
        }
        else {
            // 蛇身：普通绿色
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }

        SDL_FRect rect{
                .x = static_cast<float>(position.x * 24), .y = static_cast<float>(position.y * 24), .w = 24, .h = 24};
        SDL_RenderFillRect(renderer, &rect);
    }
}
