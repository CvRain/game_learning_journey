//
// Created by cvrain on 2025/12/27.
//

module;

#include <SDL3/SDL.h>
#include <array>
#include <format>
#include <stdexcept>
#include <string_view>

export module SomeRectangle.Application;

export class Application {
public:
    explicit Application(std::string_view window_title, int window_width, int window_height);
    ~Application();
    static auto handle_event(const SDL_Event *event) -> SDL_AppResult;
    [[nodiscard]] auto update() const -> SDL_AppResult;

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    const std::string_view window_title;
    const int window_width;
    const int window_height;
};

Application::Application(const std::string_view window_title, const int window_width, const int window_height) :
    window_title(window_title), window_width(window_width), window_height(window_height) {
    SDL_SetAppMetadata("Example Renderer Rectangle", "1.0", "com.claude-rainer.renderer-rectangle");

    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("Couldn't initialize SDL {}", SDL_GetError());
        SDL_Log("%s", result.data());
        throw std::runtime_error(result);
    }
    if (not SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, 0, &window, &renderer)) {
        const auto result = std::format("Couldn't create window and renderer: {}", SDL_GetError());
        SDL_Log("%s", result.data());
        throw std::runtime_error(result);
    }
    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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


auto Application::update() const -> SDL_AppResult {
    std::array<SDL_FRect, 16> rects{};
    const auto now = SDL_GetTicks();

    const float direction = (now % 2000) >= 1000 ? 1.0f : -1.0f;
    const float scale = static_cast<float>(now % 1000) / 500.0f * direction;

    // a basic window with background color
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);


    rects.begin()->x = 400;
    rects.begin()->y = 50;
    rects.begin()->w = 100 + (100 * scale);
    rects.begin()->h = 50 + (50 * scale);
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
    SDL_RenderFillRect(renderer, rects.begin());

    for (int i = 0; i < rects.size(); i++) {
        const auto w = static_cast<float>(window_width) / rects.size();
        const auto h = 8.0f * static_cast<float>(i);
        rects.at(i).w = i * w;
        rects.at(i).h = window_height - h;
        rects.at(i).x = w;
        rects.at(i).y = h;
    }
    SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
    SDL_RenderFillRects(renderer, rects.data(), rects.size());

    rects.begin()->x = 100;
    rects.begin()->y = 100;
    rects.begin()->w = 100 + (100 * scale);
    rects.begin()->h = 100 + (100 * scale);
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderRect(renderer, rects.begin());

    for (int i = 0; i < 3; i++) {
        const float size = (static_cast<float>(i) + 1.0f) * 50.0f;
        rects.at(i).w = size + (size * scale);
        rects.at(i).h = size + (size * scale);
        rects.at(i).x = (static_cast<float>(window_width) - rects.at(i).w) / 2;
        rects.at(i).y = (static_cast<float>(window_height) - rects.at(i).h) / 2;
    }
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    SDL_RenderRects(renderer, rects.data(), 3);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}
