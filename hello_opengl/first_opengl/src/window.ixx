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


        // 在窗口中展示两个不同的三角形
        // 创建第一个三角形的顶点数据
        std::vector<float> vertices_1{
                -0.9f,  -0.5f, 0.0f, // 左下角
                0.0f,   -0.5f, 0.0f, // 右下角
                -0.45f, 0.5f,  0.0f // 顶部
        };

        // 创建第二个三角形的顶点数据
        std::vector<float> vertices_2{
                0.0f,  -0.5f, 0.0f, // 左下角
                0.9f,  -0.5f, 0.0f, // 右下角
                0.45f, 0.5f,  0.0f // 顶部
        };


        // 两个VAO和VBO，分别对应两个三角形
        unsigned int VAO_1, VAO_2;
        unsigned int VBO_1, VBO_2;

        // 两个着色器程序，对应不同的颜色
        unsigned int shader_program_1; // 橙色
        unsigned int shader_program_2; // 黄色

        auto window_init() -> void;
    };


    Window::Window(const std::string_view &title, const int width, const int height) :
        window_title(title), window_width(width), window_height(height) {
        window_init();

        // 编译顶点着色器（两个三角形共用）
        const auto vertex_shader_path = "./res/shader/first_vert.glsl";
        const auto vertex_shader = shader_compiler(read_source_code(vertex_shader_path), GL_VERTEX_SHADER);

        // 创建第一个着色器程序（橙色）
        const auto fragment_shader_1_path = "./res/shader/first_frag.glsl";
        const auto fragment_shader_1 = shader_compiler(read_source_code(fragment_shader_1_path), GL_FRAGMENT_SHADER);
        shader_program_1 = glCreateProgram();
        glAttachShader(shader_program_1, vertex_shader);
        glAttachShader(shader_program_1, fragment_shader_1);
        glLinkProgram(shader_program_1);

        // 检查第一个程序链接状态
        GLint success;
        glGetProgramiv(shader_program_1, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetProgramInfoLog(shader_program_1, 512, nullptr, info_log);
            std::string error_message = "Shader program 1 linking failed: ";
            error_message += info_log;
            throw std::runtime_error(error_message);
        }

        // 创建第二个着色器程序（黄色）
        const auto fragment_shader_2_path = "./res/shader/second_frag.glsl";
        const auto fragment_shader_2 = shader_compiler(read_source_code(fragment_shader_2_path), GL_FRAGMENT_SHADER);
        shader_program_2 = glCreateProgram();
        glAttachShader(shader_program_2, vertex_shader);
        glAttachShader(shader_program_2, fragment_shader_2);
        glLinkProgram(shader_program_2);

        // 检查第二个程序链接状态
        glGetProgramiv(shader_program_2, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetProgramInfoLog(shader_program_2, 512, nullptr, info_log);
            std::string error_message = "Shader program 2 linking failed: ";
            error_message += info_log;
            throw std::runtime_error(error_message);
        }

        // 删除着色器对象
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader_1);
        glDeleteShader(fragment_shader_2);

        // 设置第一个三角形的VAO和VBO
        glGenVertexArrays(1, &VAO_1);
        glGenBuffers(1, &VBO_1);

        glBindVertexArray(VAO_1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_1);
        glBufferData(GL_ARRAY_BUFFER, vertices_1.size() * sizeof(float), vertices_1.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(0));
        glEnableVertexAttribArray(0);

        // 设置第二个三角形的VAO和VBO
        glGenVertexArrays(1, &VAO_2);
        glGenBuffers(1, &VBO_2);

        glBindVertexArray(VAO_2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
        glBufferData(GL_ARRAY_BUFFER, vertices_2.size() * sizeof(float), vertices_2.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(0));
        glEnableVertexAttribArray(0);

        // 解绑
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Window::~Window() {
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

        // 绘制第一个三角形（橙色）
        glUseProgram(shader_program_1);
        glBindVertexArray(VAO_1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 绘制第二个三角形（黄色）
        glUseProgram(shader_program_2);
        glBindVertexArray(VAO_2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap the buffers
        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
    }

    auto Window::window_init() -> void {
        // SDL_Init returns 0 on success, negative on failure.
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
            throw std::runtime_error(result);
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window = SDL_CreateWindow(window_title.data(), window_width, window_height,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
        glViewport(0, 0, window_width, window_height);

        SDL_GL_SetSwapInterval(1); // Enable VSyn c
    }
} // namespace first_opengl
