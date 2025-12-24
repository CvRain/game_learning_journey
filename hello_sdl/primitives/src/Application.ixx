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
    SDL_FPoint points[500]{};

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
    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    for (int i = 0; i < 500; ++i) {
        points[i].x = (SDL_randf() * 440.0f) + 100.0f;
        points[i].y = (SDL_randf() * 280.0f) + 100.0f;
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
    SDL_FRect rect;

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 48, 52, 70, SDL_ALPHA_OPAQUE);  /* dark gray, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */

    /* draw a filled rectangle in the middle of the canvas. */
    SDL_SetRenderDrawColor(renderer, 65, 69, 89, SDL_ALPHA_OPAQUE);  /* blue, full alpha */
    rect.x = rect.y = 100;
    rect.w = 440;
    rect.h = 280;
    SDL_RenderFillRect(renderer, &rect);

    /* draw some points across the canvas. */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  /* red, full alpha */
    SDL_RenderPoints(renderer, points, SDL_arraysize(points));

    /* draw a unfilled rectangle in-set a little bit. */
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);  /* green, full alpha */
    rect.x += 30;
    rect.y += 30;
    rect.w -= 60;
    rect.h -= 60;
    SDL_RenderRect(renderer, &rect);

    /* draw two lines in an X across the whole canvas. */
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);  /* yellow, full alpha */
    SDL_RenderLine(renderer, 0, 0, 640, 480);
    SDL_RenderLine(renderer, 0, 480, 640, 0);

    SDL_RenderPresent(renderer);  /* put it all on the screen! */

    return SDL_APP_CONTINUE;
}


auto Application::handle_event(const SDL_Event *event) -> void {
    // Currently no special event handling; quit is managed in main loop.
}
