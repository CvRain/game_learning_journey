//
// Created by cvrain on 2025/12/24.
//
module;
#include <SDL3/SDL.h>
#include <format>
#include <string_view>

export module Primitives.Application;

export class Application {
public:
    explicit Application(const std::string_view &title, int width, int height);
    ~Application();
    auto update() -> SDL_AppResult;
    auto handle_event(const SDL_Event *event) -> void;

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    std::array<SDL_FPoint, 100> points{};

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

    for (auto &[x, y]: points) {
        x = (SDL_randf() * 440.0f) + 100.0f;
        y = (SDL_randf() * 280.0f) + 100.0f;
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
    SDL_SetRenderDrawColor(renderer, 48, 52, 70, SDL_ALPHA_OPAQUE); /* dark gray, full alpha */
    SDL_RenderClear(renderer); /* start with a blank canvas. */

    /* draw a filled rectangle in the middle of the canvas. */
    SDL_SetRenderDrawColor(renderer, 65, 69, 89, SDL_ALPHA_OPAQUE); /* blue, full alpha */
    rect.x = rect.y = 0;
    rect.w = this->window_width;
    rect.h = this->window_height;
    SDL_RenderFillRect(renderer, &rect);

    /* draw some points across the canvas with gradient colors and alpha. */
    const auto ticks = SDL_GetTicks();
    const auto current_time = static_cast<double>(ticks) / 1000.0f;

    for (size_t i = 0; i < points.size(); ++i) {
        // Calculate position-based color and alpha for gradient effect
        const auto point_offset = static_cast<double>(i) / points.size();
        const auto time_offset = current_time * 0.5; // Slower time variation

        // Create colors that transition from white towards different hues
        const auto red = static_cast<Uint8>(255 * (0.7 + 0.3 * SDL_sin(time_offset + point_offset * SDL_PI_D)));
        const auto green = static_cast<Uint8>(255 * (0.7 + 0.3 * SDL_sin(time_offset + point_offset * SDL_PI_D * 2.0)));
        const auto blue = static_cast<Uint8>(255 * (0.7 + 0.3 * SDL_sin(time_offset + point_offset * SDL_PI_D * 3.0)));

        // Vary alpha to create transparency effect
        const auto alpha = static_cast<Uint8>(128 + 127 * SDL_sin(time_offset * 2.0 + point_offset * SDL_PI_D * 2.0));

        SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
        SDL_RenderPoint(renderer, points[i].x, points[i].y);
    }

    /* draw a unfilled rectangle in-set a little bit. */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* green, full alpha */
    rect.x += 30;
    rect.y += 30;
    rect.w -= 60;
    rect.h -= 60;
    SDL_RenderRect(renderer, &rect);

    /* draw two lines in an X across the whole canvas. */
    // SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);  /* yellow, full alpha */
    // SDL_RenderLine(renderer, 0, 0, 640, 480);
    // SDL_RenderLine(renderer, 0, 480, 640, 0);

    SDL_RenderPresent(renderer); /* put it all on the screen! */

    return SDL_APP_CONTINUE;
}


auto Application::handle_event(const SDL_Event *event) -> void {
    // Currently no special event handling; quit is managed in main loop.
}
