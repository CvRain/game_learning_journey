#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <memory>

import snake.application;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    auto application = std::make_unique<Application>();
    if (not application) {
        throw std::runtime_error("Failed to create application");
    }
    *appstate = application.release();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto *application = static_cast<Application *>(appstate);
    return application->handle_event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto *application = static_cast<Application *>(appstate);
    return application->handle_iteration();
}

void SDL_AppQuit(void *appstate, const SDL_AppResult result) {
    const auto *application = static_cast<Application *>(appstate);
    delete application;
}