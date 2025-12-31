module;

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <string>
#include <string_view>
#include <format>
#include <stdexcept>

export module first_opengl.window;

/**
 * Window 类 - 管理 SDL 窗口和 OpenGL 上下文
 * 使用 RAII 管理窗口资源
 */
export class Window {
public:
    /**
     * 创建窗口
     * @param title 窗口标题
     * @param width 窗口宽度
     * @param height 窗口高度
     */
    Window(std::string_view title, int width, int height);

    ~Window();

    // 禁止拷贝和移动
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    /**
     * 交换前后缓冲区
     */
    void swap_buffers() const;

    /**
     * 获取窗口尺寸
     */
    [[nodiscard]] auto get_size() const -> std::pair<int, int>;

    /**
     * 设置视口大小
     */
    void set_viewport(int width, int height) const;

    /**
     * 获取 SDL 窗口指针（用于事件处理）
     */
    [[nodiscard]] auto get_sdl_window() const -> SDL_Window * {
        return window;
    }

private:
    SDL_Window *window{nullptr};
    SDL_GLContext gl_context{nullptr};

    void init_sdl();
    void create_window(std::string_view title, int width, int height);
    void create_gl_context();
    void init_glad();
};

Window::Window(const std::string_view title, const int width, const int height) {
    init_sdl();
    create_window(title, width, height);
    create_gl_context();
    init_glad();
    set_viewport(width, height);

    // 启用 VSync
    SDL_GL_SetSwapInterval(1);
}

Window::~Window() {
    if (gl_context != nullptr) {
        SDL_GL_DestroyContext(gl_context);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

void Window::swap_buffers() const {
    SDL_GL_SwapWindow(window);
}

auto Window::get_size() const -> std::pair<int, int> {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    return {width, height};
}

void Window::set_viewport(const int width, const int height) const {
    glViewport(0, 0, width, height);
}

void Window::init_sdl() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(
            std::format("SDL_Init Error: {}", SDL_GetError())
        );
    }

    // 设置 OpenGL 版本
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // 启用双缓冲
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void Window::create_window(const std::string_view title, const int width, const int height) {
    window = SDL_CreateWindow(
        title.data(),
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr) {
        throw std::runtime_error(
            std::format("SDL_CreateWindow Error: {}", SDL_GetError())
        );
    }
}

void Window::create_gl_context() {
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        throw std::runtime_error(
            std::format("SDL_GL_CreateContext Error: {}", SDL_GetError())
        );
    }
}

void Window::init_glad() {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

