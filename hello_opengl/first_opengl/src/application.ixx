module;

#include <SDL3/SDL.h>
#include <string_view>
#include <memory>
#include <array>

export module first_opengl.application;

import first_opengl.window;
import first_opengl.shader;
import first_opengl.mesh;
import first_opengl.renderer;

/**
 * Application 类 - 应用程序主类
 * 负责协调窗口、渲染器、着色器和网格
 */
export class Application {
public:
    explicit Application(std::string_view title, int width, int height);

    ~Application() = default;

    /**
     * 处理 SDL 事件
     */
    auto handle_event(SDL_Event *event) -> SDL_AppResult;

    /**
     * 处理每一帧的渲染
     */
    auto handle_iteration() -> SDL_AppResult;

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Mesh> triangle_mesh;

    // 三角形顶点数据
    static constexpr std::array<float, 9> triangle_vertices = {
        -0.5f, -0.5f, 0.0f,  // 左下
         0.5f, -0.5f, 0.0f,  // 右下
         0.0f,  0.5f, 0.0f   // 顶部
    };

    void init_resources();
};

Application::Application(const std::string_view title, const int width, const int height) {
    // 创建窗口（自动初始化 SDL 和 OpenGL）
    window = std::make_unique<Window>(title, width, height);

    // 初始化资源
    init_resources();

    SDL_Log("Application initialized successfully");
}

void Application::init_resources() {
    // 加载并编译着色器
    shader = std::make_unique<Shader>(
        "./res/shader/first_vert.glsl",
        "./res/shader/first_frag.glsl"
    );

    // 创建三角形网格
    triangle_mesh = std::make_unique<Mesh>(triangle_vertices);
}

auto Application::handle_event(SDL_Event *event) -> SDL_AppResult {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;

        case SDL_EVENT_KEY_DOWN:
            // 用户按下 ESC 键，则退出程序
            if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
                return SDL_APP_SUCCESS;
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            // 窗口大小改变时，更新 OpenGL 视口
            {
                const auto [new_width, new_height] = window->get_size();
                window->set_viewport(new_width, new_height);
                SDL_Log("Window resized to %dx%d", new_width, new_height);
            }
            break;

        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

auto Application::handle_iteration() -> SDL_AppResult {
    // 清除屏幕
    Renderer::clear(0.1f, 0.2f, 0.3f, 1.0f);

    // 渲染三角形
    Renderer::draw(*shader, *triangle_mesh);

    // 交换缓冲区
    window->swap_buffers();

    return SDL_APP_CONTINUE;
}
