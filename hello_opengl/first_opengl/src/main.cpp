#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <exception>
#include <memory>

import first_opengl.window;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_Log("SDL_AppInit called");
    try {
        auto application = std::make_unique<first_opengl::Window>("first opengl", 640, 480);
        *appstate = application.release();
        SDL_Log("Application created successfully");
        return SDL_APP_CONTINUE;
    }
    catch (const std::exception &e) {
        SDL_Log("Application init failed: %s", e.what());
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    const auto application = static_cast<first_opengl::Window *>(appstate);
    return application->handle_event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto application = static_cast<first_opengl::Window *>(appstate);
    return application->handle_iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_Log("SDL_AppQuit called");
    const auto application = static_cast<first_opengl::Window *>(appstate);
    delete application;
    SDL_Log("Application destroyed successfully");
}
