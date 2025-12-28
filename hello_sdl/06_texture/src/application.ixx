module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <string_view>

export module texture06.application;

export class Application {
public:
    explicit Application(std::string_view title, int width, int height);
    ~Application();
    SDL_AppResult handle_event(SDL_Event *event);
    SDL_AppResult handle_iteration();

private:
    const std::string_view window_title = "SDL3 Texture Example";
    const int window_width = 800;
    const int window_height = 600;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

    int texture_width = 0;
    int texture_height = 0;
};

Application::Application(const std::string_view title, const int width, const int height) :
    window_title(title), window_width(width), window_height(height) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Could not initialize SDL: %s", SDL_GetError());
        throw std::runtime_error("Could not initialize SDL");
    }
    if (not SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                        &renderer)) {
        SDL_Log("Could not create window and renderer: %s", SDL_GetError());
        throw std::runtime_error("Could not create window and renderer");
    }

    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    const std::string texture_path = "./res/sample.png";
    SDL_Surface* surface = IMG_Load(texture_path.c_str());
    if (!surface) {
        SDL_Log("Could not load image %s: %s", texture_path.c_str(), SDL_GetError());
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Could not create texture: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    texture_width = surface->w;
    texture_height = surface->h;
    SDL_DestroySurface(surface);
}

Application::~Application() {
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    if (texture)
        SDL_DestroyTexture(texture);
    SDL_Quit();
}

SDL_AppResult Application::handle_event(SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::handle_iteration() {
    // SDL3: SDL_GetTicks returns Uint64 milliseconds
    const double now = static_cast<double>(SDL_GetTicks()) / 1000.0;

    // Create smooth color transitions using sine waves
    const auto red = static_cast<float>(0.5 + 0.5 * SDL_sin(now));
    const auto green = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2.0 / 3.0));
    const auto blue = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4.0 / 3.0));

    // SDL3: Use SetRenderDrawColorFloat (0.0~1.0 range)
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);

    SDL_RenderClear(renderer);

    const auto ticks = SDL_GetTicks();
    const auto direction = ((ticks % 2000) >= 1000) ? 1.0f : -1.0f;
    const auto scale = (static_cast<float>(static_cast<int>(ticks % 1000) - 500) / 500.0f) * direction;


    SDL_FRect rect;
    /* top left */
    rect.x = (100.0f * scale);
    rect.y = 0.0f;
    rect.w = static_cast<float>(texture_width);
    rect.h = static_cast<float>(texture_height);
    SDL_RenderTexture(renderer, texture, nullptr, &rect);

    /* bottom right */
    rect.x = static_cast<float>(window_width - texture_width) - (100.0f * scale);
    rect.y = static_cast<float>(window_height - texture_height);
    rect.w = static_cast<float>(texture_width);
    rect.h = static_cast<float>(texture_height);
    SDL_RenderTexture(renderer, texture, nullptr, &rect);

    /* center this one. */
    rect.x = (window_width - texture_width) / 2.0f;
    rect.y = (window_height - texture_height) / 2.0f;
    rect.w = static_cast<float>(texture_width) * static_cast<float>(scale);
    rect.h = static_cast<float>(texture_height) * static_cast<float>(scale);
    SDL_RenderTexture(renderer, texture, nullptr, &rect);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}
