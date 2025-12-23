//
// Created by cvrain on 2025/12/23.
//

#include "application.hpp"
#include <cmath>
#include <iostream>

bool Application::init() {
    // SDL3: SDL_Init 返回 bool (true=成功)
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // SDL3: SDL_CreateWindowAndRenderer 签名变化
    if (!SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
        std::cerr << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    return true;
}

SDL_AppResult Application::update() {
    // SDL3: SDL_GetTicks 返回 Uint64 毫秒
    const double now = static_cast<double>(SDL_GetTicks()) / 1000.0;

    // 使用正弦波创建平滑的颜色渐变
    const auto red = static_cast<float>(0.5 + 0.5 * SDL_sin(now));
    const auto green = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2.0 / 3.0));
    const auto blue = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4.0 / 3.0));

    // SDL3: 使用 SetRenderDrawColorFloat (0.0~1.0 范围)
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE; /* 继续运行 */
}

void Application::handleEvents(const SDL_Event *event) {
    // For now, nothing special here; quit is handled in main loop.
}

Application::~Application() {
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}
