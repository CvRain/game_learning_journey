module;
#include <SDL3/SDL.h>

export module first_opengl.app;

export namespace first_opengl {
    class Application {
    public:
        virtual ~Application() = default;

        virtual void on_init() = 0;

        virtual void on_update(double delta_time) = 0;

        virtual auto on_event(const SDL_Event &event) -> SDL_AppResult = 0;

        virtual void on_quit() = 0;
    };
} // namespace first_opengl
