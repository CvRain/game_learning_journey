module;

#include <SDL3/SDL.h>
#include <format>
#include <glad/glad.h>
#include <string_view>

export module first_opengl.application;

export class Application {
public:
    explicit Application(std::string_view title, int width, int height);
    ~Application();
    auto handle_event(SDL_Event *event) -> SDL_AppResult;
    auto handle_iteration() -> SDL_AppResult;

private:
    SDL_Window *window{nullptr};
    SDL_GLContext gl_context{nullptr};

    const std::string_view window_title;
    const int width;
    const int height;
};

Application::Application(std::string_view title, int w, int h) : window_title(title), width(w), height(h) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(window_title.data(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    SDL_GL_SetSwapInterval(1); // Enable VSync
}

Application::~Application() {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

auto Application::handle_event(SDL_Event *event) -> SDL_AppResult {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

auto Application::handle_iteration() -> SDL_AppResult {
    // Clear the screen to a solid color
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap the buffers
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}
