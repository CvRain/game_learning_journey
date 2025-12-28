module;
#include <SDL3/SDL.h>
#include <stdexcept>
#include <string_view>

export module streaming_texture.application;


export class Application {
public:
    Application(std::string_view title, int width, int height);
    ~Application();

    SDL_AppResult handle_event(SDL_Event *event);
    SDL_AppResult handle_iteration();

private:
    const std::string_view window_title;
    const int window_width;
    const int window_height;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
};


Application::Application(std::string_view title, int width, int height) :
    window_title{title}, window_width{width}, window_height{height} {
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

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 150, 150);
    if (not texture) {
        SDL_Log("Could not create texture: %s", SDL_GetError());
        throw std::runtime_error("Could not create texture");
    }
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

    SDL_Surface *surface = nullptr;
    if (SDL_LockTextureToSurface(texture, nullptr, &surface)) {
        SDL_Rect rect;
        SDL_FillSurfaceRect(surface, nullptr, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), nullptr, 0, 0, 0));
        rect.w = 150.0f;
        rect.h = 15;
        rect.x = 0;
        rect.y = static_cast<int>(static_cast<float>(window_width - rect.h) * ((scale + 1.0f) / 2.0f));
        SDL_FillSurfaceRect(surface, &rect,
                            SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), nullptr, 0, 255,
                                       0)); /* make a strip of the surface green */
        SDL_UnlockTexture(texture);
    }

    SDL_FRect dst_rect;
    dst_rect.x = static_cast<float>(window_width - 150) / 2.0f;
    dst_rect.y = static_cast<float>(window_height - 150) / 2.0f;
    dst_rect.w = dst_rect.h = 150.0f;
    SDL_RenderTexture(renderer, texture, nullptr, &dst_rect);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}
