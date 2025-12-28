#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

import SomeRectangle.Application;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    auto apple = std::make_unique<Application>("Some Rectangle", 800, 600);
    if (not apple) {
        SDL_Log("Failed to create Application instance!");
        return SDL_APP_FAILURE;
    }
    *appstate = apple.release();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *app_state, SDL_Event *event) {
    auto *application = static_cast<Application *>(app_state);
    const auto result = application->handle_event(event);
    if (result != SDL_APP_CONTINUE) {
        return result;
    }
    application->handle_event(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *app_state) {
    auto *application = static_cast<Application *>(app_state);
    application->update();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    const auto *application = static_cast<Application *>(appstate);
    delete application;
}
