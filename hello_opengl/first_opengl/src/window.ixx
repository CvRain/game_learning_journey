module;
#include <SDL3/SDL.h>
#include <cmath>
#include <filesystem>
#include <format>
#include <glad/glad.h>
#include <stb_image.h>
#include <stdexcept>
#include <string_view>
#include <vector>

export module first_opengl.window;

import first_opengl.shader;
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


        std::shared_ptr<Shader> shader_program = nullptr;

        unsigned int vertex_array_object{};
        unsigned int vertex_buffer_object{};
        unsigned int element_array_buffer{};
        unsigned int texture_1{};
        unsigned int texture_2{};

        auto window_init() -> void;

        // 绘制一个矩形所用的顶点和颜色数据
        const std::vector<float> vertices = {
            // positions          // colors           // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
        };

        // 绘制矩形所用的索引
        const std::vector<unsigned int> indices = {
            0, 1, 3, // 第一个三角形
            1, 2, 3 // 第二个三角形
        };
    };


    Window::Window(const std::string_view &title, const int width, const int height) : window_title(title),
        window_width(width), window_height(height) {
        window_init();

        shader_program = std::make_shared<Shader>("./res/shader/first_vert.glsl", "./res/shader/first_frag.glsl");
        shader_program->use();

        // 设置顶点数组对象和顶点缓冲对象
        glGenVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);

        glGenBuffers(1, &vertex_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &element_array_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // 翻转Y轴，使图片坐标系与OpenGL纹理坐标系一致
        stbi_set_flip_vertically_on_load(true);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture_1);
        glBindTexture(GL_TEXTURE_2D, texture_1);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        if (const auto img_data = load_image("./res/image/wall.jpg"); img_data.data != nullptr) {
            const auto format = get_image_format(img_data);
            glTexImage2D(GL_TEXTURE_2D, 0, format, img_data.width, img_data.height, 0, format, GL_UNSIGNED_BYTE,
                         img_data.data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(img_data.data);
        }
        else {
            stbi_image_free(img_data.data);
            throw std::runtime_error("Failed to load texture image");
        }

        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &texture_2);
        glBindTexture(GL_TEXTURE_2D, texture_2);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (const auto img_data = load_image("./res/image/sample.png"); img_data.data != nullptr) {
            const auto format = get_image_format(img_data);
            glTexImage2D(GL_TEXTURE_2D, 0, format, img_data.width, img_data.height, 0, format, GL_UNSIGNED_BYTE,
                         img_data.data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(img_data.data);
        }
        else {
            stbi_image_free(img_data.data);
            throw std::runtime_error("Failed to load texture image");
        }

        shader_program->set_int("ourTexture", 0);
        shader_program->set_int("texture2", 1);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_2);
        shader_program->use();

        glBindVertexArray(vertex_array_object);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);


        // Swap the buffers
        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
    }

    auto Window::window_init() -> void {
        // SDL_Init returns 0 on success, negative on failure.
        if (not SDL_Init(SDL_INIT_VIDEO)) {
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
