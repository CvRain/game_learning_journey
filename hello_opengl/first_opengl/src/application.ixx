module;

#include <SDL3/SDL.h>
#include <format>
#include <glad/glad.h>
#include <string_view>
#include <vector>

export module first_opengl.application;

import first_opengl.file_operation;

export class Application {
public:
    explicit Application(std::string_view title, int width, int height);

    ~Application();

    auto handle_event(SDL_Event *event) -> SDL_AppResult;

    auto handle_iteration() -> SDL_AppResult;

private:
    SDL_Window *window{nullptr};
    SDL_GLContext gl_context{nullptr};

    const std::string_view window_title;
    const int width;
    const int height;

    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    GLuint VBO;
    GLuint VAO;
    GLuint shader_program;

    auto window_init() -> SDL_AppResult;
};

Application::Application(const std::string_view title, const int width, const int height)
    : window_title(title), width(width), height(height) {
    window_init();

    shader_program = glCreateProgram();

    const auto vertex_shader = shader_compiler(read_source_code("./res/shader/first_vert.glsl"), GL_VERTEX_SHADER);
    const auto fragment_shader = shader_compiler(read_source_code("./res/shader/first_frag.glsl"), GL_FRAGMENT_SHADER);

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Application::~Application() {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

auto Application::handle_event(SDL_Event *event) -> SDL_AppResult {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            // 用户按下esc按键，则退出程序
            if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
                return SDL_APP_SUCCESS;
            }
            break;
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            // 窗口大小改变时，更新OpenGL视口
        {
            int new_width, new_height;
            SDL_GetWindowSize(window, &new_width, &new_height);
            glViewport(0, 0, new_width, new_height);
            SDL_Log("Window resized to %dx%d", new_width, new_height);
        }
        break;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

auto Application::handle_iteration() -> SDL_AppResult {
    // Clear the screen to a solid color
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the triangle
    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // Swap the buffers
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}


auto Application::window_init() -> SDL_AppResult {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(window_title.data(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        const auto result = std::format("SDL_CreateWindow Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        const auto result = std::format("SDL_GL_CreateContext Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }

    if (not gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // 设置初始视口大小
    glViewport(0, 0, width, height);

    SDL_GL_SetSwapInterval(1); // Enable VSync
    return SDL_APP_CONTINUE;
}
