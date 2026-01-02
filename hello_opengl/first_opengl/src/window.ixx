module;

#include <SDL3/SDL.h>
#include <filesystem>
#include <format>
#include <glad/glad.h>
#include <stdexcept>
#include <string_view>
#include <vector>


export module first_opengl.window;

import first_opengl.file_operation;

export namespace first_opengl {
    class Window {
    public:
        explicit Window(const std::string_view &title, int width, int height);
        ~Window();
        auto handle_event(SDL_Event *event) -> SDL_AppResult;
        auto handle_iterate() -> SDL_AppResult;

    private:
        const std::string_view window_title;
        const int window_width;
        const int window_height;

        SDL_Window *window = nullptr;
        SDL_GLContext gl_context = nullptr;

        std::vector<float> vertices = {
                0.5f,  0.5f,  0.0f, // 右上角
                0.5f,  -0.5f, 0.0f, // 右下角
                -0.5f, -0.5f, 0.0f, // 左下角
                -0.5f, 0.5f,  0.0f // 左上角
        };

        std::vector<GLuint> indices = {
                // 注意索引从0开始!
                // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
                // 这样可以由下标代表顶点组合成矩形

                0, 1, 3, // 第一个三角形
                1, 2, 3 // 第二个三角形
        };

        GLuint VBO;
        GLuint VAO;
        GLuint EBO;
        GLuint shader_program;

        static void validate_program(GLuint program);
    };

    void Window::validate_program(const GLuint program) {
        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            char info_log[1024];
            glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
            throw std::runtime_error(std::string("Program link failed: ") + info_log);
        }
    }

    Window::Window(const std::string_view &title, const int width, const int height) :
        window_title(title), window_width(width), window_height(height) {
        // SDL_Init returns 0 on success, negative on failure.
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

        SDL_GL_SetSwapInterval(1); // Enable VSyn                                                                                     c

        shader_program = glCreateProgram();

        const auto vert_path = "res/shader/first_vert.glsl";
        const auto frag_path = "res/shader/first_frag.glsl";

        const auto vertex_shader = shader_compiler(read_source_code(vert_path), GL_VERTEX_SHADER);
        const auto fragment_shader = shader_compiler(read_source_code(frag_path), GL_FRAGMENT_SHADER);

        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);

        glLinkProgram(shader_program);
        validate_program(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // 注意：必须先解绑VAO，再解绑其他缓冲区
        // EBO的绑定会保存在VAO中，但GL_ARRAY_BUFFER不会
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 不要在这里解绑EBO，因为VAO会记住EBO的绑定
    }

    Window::~Window() {
        // Clean up GL objects while the context still exists.
        if (shader_program)
            glDeleteProgram(shader_program);
        if (EBO)
            glDeleteBuffers(1, &EBO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
        if (VAO)
            glDeleteVertexArrays(1, &VAO);

        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    auto Window::handle_event(SDL_Event *event) -> SDL_AppResult {
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

    auto Window::handle_iterate() -> SDL_AppResult {
        // Clear the screen to a solid color
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap the buffers
        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
    }


} // namespace first_opengl
