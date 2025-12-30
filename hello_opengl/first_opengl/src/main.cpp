#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <memory>

// Force rebuild
import first_opengl.application;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_Log("SDL_AppInit called");
    try {
        auto application = std::make_unique<Application>("first opengl", 640, 480);
        *appstate = application.release();
        SDL_Log("Application created successfully");
        return SDL_APP_CONTINUE;
    } catch (const std::exception &e) {
        SDL_Log("Application init failed: %s", e.what());
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    const auto application = static_cast<Application *>(appstate);
    return application->handle_event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto application = static_cast<Application *>(appstate);
    return application->handle_iteration();
}

void SDL_AppQuit(void *appstate, const SDL_AppResult result) {
    const auto application = static_cast<Application *>(appstate);
    delete application;
}
