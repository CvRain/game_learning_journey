#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <memory>

import Lines.Application;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    auto application = std::make_unique<Application>("Hello Lines", 640, 480);
    if (not application) {
        SDL_Log("Failed to create Application instance!");
        return SDL_APP_FAILURE;
    }
    *appstate = application.release();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto *application = static_cast<Application *>(appstate);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    application->handle_event(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto *application = static_cast<Application *>(appstate);
    return application->update();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    const auto *application = static_cast<Application *>(appstate);
    delete application;
}