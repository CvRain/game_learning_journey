#define SDL_MAIN_USE_CALLBACKS 1

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

import woodeneye.application;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    try {
        auto application = std::make_unique<Application>("wooden_eye", 640, 480);
        *appstate = application.release();
    }
    catch (const std::exception &e) {
        SDL_Log("Failed to initialize application: %s", e.what());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto application = static_cast<Application *>(appstate);
    return application->handle_iteration();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    const auto application = static_cast<Application *>(appstate);
    return application->handle_event(event);
}

void SDL_AppQuit(void *appstate, const SDL_AppResult result) {
    const auto application = static_cast<Application *>(appstate);
    delete application;
}
