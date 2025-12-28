module;
#include <SDL3/SDL.h>
#include <format>
#include <string_view>

export module snake.application;

export class Application {
public:
    explicit Application(std::string_view window_title = "Snake", int width = 640, int height = 480);
    ~Application();
    auto handle_event(SDL_Event *event) -> SDL_AppResult;
    auto handle_iteration() -> SDL_AppResult;

private:
    const std::string_view window_title;
    const int window_width = 640;
    const int window_height = 480;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};

Application::Application(const std::string_view window_title, const int width, const int height) :
    window_title(window_title), window_width(width), window_height(height) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
        SDL_Log(result.data());
        throw std::runtime_error(result);
    }

    if (not SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                        &renderer)) {
        const auto result = std::format("SDL_CreateWindowAndRenderer Error: {}", SDL_GetError());
        SDL_Log(result.data());
        throw std::runtime_error(result);
    }
    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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
    return SDL_APP_CONTINUE;
}

auto Application::handle_iteration() -> SDL_AppResult {
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}
