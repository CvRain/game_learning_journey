#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

import texture06.application;


SDL_AppResult SDL_AppInit(void **app_state, int argc, char **argv) {
    auto *app = new Application("SDL3 Texture Example", 800, 600);
    *app_state = app;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *app_state, SDL_Event *event) {
    auto *application = static_cast<Application *>(app_state);
    return application->handle_event(event);
}

SDL_AppResult SDL_AppIterate(void *app_state) {
    auto *application = static_cast<Application *>(app_state);
    return application->handle_iteration();
}

void SDL_AppQuit(void *app_state, SDL_AppResult result) {
    auto *application = static_cast<Application *>(app_state);
    delete application;
}
