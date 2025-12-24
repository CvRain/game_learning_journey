//
// Created by cvrain on 2025/12/24.
//
module;
#include <SDL3/SDL.h>
#include <format>
#include <string_view>

export module Application;

export class Application {
public:
    explicit Application(const std::string_view &title, int width, int height);
    ~Application();
    auto update() -> SDL_AppResult;
    auto handle_event(const SDL_Event *event) -> void;

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    const std::string_view window_title;
    const int window_width;
    const int window_height;
};

Application::Application(const std::string_view &title, const int width, const int height) :
    window_title(title), window_width(width), window_height(height) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("SDL initialization failed: {}", SDL_GetError());
        throw std::runtime_error(result);
    }
    if (not SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                        &renderer)) {
        const auto result = std::format("SDL CreateWindowAndRenderer failed: {}", SDL_GetError());
        SDL_Quit();
        throw std::runtime_error(result);
    }
}


Application::~Application() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

auto Application::update() -> SDL_AppResult {
    const double now = static_cast<double>(SDL_GetTicks()) / 1000.0;

    const auto red = static_cast<float>(0.5 + 0.5 * SDL_sin(now));
    const auto green = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2.0 / 3.0));
    const auto blue = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4.0 / 3.0));

    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}


auto Application::handle_event(const SDL_Event *event) -> void {
    // Currently no special event handling; quit is managed in main loop.
}
