module;
#include <SDL3/SDL.h>
#include <format>
#include <glad/glad.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

export module opengl_sandbox.window;

import opengl_sandbox.app;

export namespace opengl_sandbox {
    class Window {
    public:
        explicit Window(const std::string_view &title, int width, int height);

        ~Window();

        auto set_application(Application *app) -> void { application = app; }

        auto handle_event(SDL_Event *event) -> SDL_AppResult;

        auto handle_iterate() -> SDL_AppResult;

        [[nodiscard]] auto get_width() const -> int { return window_width; }
        [[nodiscard]] auto get_height() const -> int { return window_height; }
        [[nodiscard]] auto get_native_window() const -> SDL_Window * { return window; }

    private:
        std::string window_title;
        int window_width;
        int window_height;

        SDL_Window *window = nullptr;
        SDL_GLContext gl_context = nullptr;

        Application *application = nullptr;

        auto window_init() -> void;
    };

    Window::Window(const std::string_view &title, const int width, const int height) :
        window_title(title), window_width(width), window_height(height) {
        window_init();
    }

    Window::~Window() {
        if (application) {
            application->on_quit();
        }
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    auto Window::handle_event(SDL_Event *event) -> SDL_AppResult {
        if (event->type == SDL_EVENT_QUIT) {
            return SDL_APP_SUCCESS;
        }

        if (event->type == SDL_EVENT_WINDOW_RESIZED || event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
            int new_width, new_height;
            SDL_GetWindowSize(window, &new_width, &new_height);
            window_width = new_width;
            window_height = new_height;
            glViewport(0, 0, new_width, new_height);
            SDL_Log("Window resized to %dx%d", new_width, new_height);
        }

        if (application) {
            return application->on_event(*event);
        }

        return SDL_APP_CONTINUE;
    }

    auto Window::handle_iterate() -> SDL_AppResult {
        // Clear background
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (application) {
            const auto current_time = SDL_GetTicks();
            // Simple delta time calculation.
            static Uint64 last_time = 0;
            if (last_time == 0)
                last_time = current_time;

            const float delta_time = static_cast<float>(current_time - last_time) / 1000.0f;
            last_time = current_time;

            application->on_update(delta_time);
        }

        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
    }

    auto Window::window_init() -> void {
        if (not SDL_Init(SDL_INIT_VIDEO)) {
            const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
            throw std::runtime_error(result);
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window = SDL_CreateWindow(window_title.c_str(), window_width, window_height,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (window == nullptr) {
            const auto result = std::format("SDL_CreateWindow Error: {}", SDL_GetError());
            throw std::runtime_error(result);
        }

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr) {
            const auto result = std::format("SDL_GL_CreateContext Error: {}", SDL_GetError());
            throw std::runtime_error(result);
        }

        if (not gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, window_width, window_height);

        SDL_SetWindowRelativeMouseMode(window, true);
        SDL_GL_SetSwapInterval(1);
    }
} // namespace opengl_sandbox
