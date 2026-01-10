#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <exception>
#include <memory>

import first_opengl.window;
import first_opengl.sandbox;

struct AppContext {
    std::unique_ptr<first_opengl::Window> window;
    std::unique_ptr<first_opengl::Sandbox> sandbox;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_Log("SDL_AppInit called");
    try {
        auto context = new AppContext();
        context->window = std::make_unique<first_opengl::Window>("first opengl", 640, 480);
        context->sandbox = std::make_unique<first_opengl::Sandbox>(*context->window);

        context->window->set_application(context->sandbox.get());
        context->sandbox->on_init();

        *appstate = context;
        SDL_Log("Application created successfully");
        return SDL_APP_CONTINUE;
    }
    catch (const std::exception &e) {
        SDL_Log("Application init failed: %s", e.what());
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    const auto context = static_cast<AppContext *>(appstate);
    return context->window->handle_event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto context = static_cast<AppContext *>(appstate);
    return context->window->handle_iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_Log("SDL_AppQuit called");
    if (appstate) {
        const auto context = static_cast<AppContext *>(appstate);
        delete context;
    }
    SDL_Log("Application destroyed successfully");
}
