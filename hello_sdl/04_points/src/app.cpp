#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

import Points.Application;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    auto application = std::make_unique<Application>("some points", 1366, 768);

    if (not application) {
        SDL_Log("Failed to create Application instance!");
        return SDL_APP_FAILURE;
    }

    *appstate = application.release();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *app_state, SDL_Event *event) {
    auto *application = static_cast<Application *>(app_state);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    application->handle_event(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *app_state) {
    auto *application = static_cast<Application *>(app_state);
    return application->update();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    const auto *application = static_cast<Application *>(appstate);
    delete application;
}
