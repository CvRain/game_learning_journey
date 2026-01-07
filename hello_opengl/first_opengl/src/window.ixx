module;
#include <SDL3/SDL.h>
#include <cmath>
#include <filesystem>
#include <format>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
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

        // 移动相关的状态变量
        glm::vec3 position{0.0f, 0.0f, 0.0f}; // 当前位置
        glm::vec3 velocity{0.0f, 0.0f, 0.0f}; // 当前速度向量
        float change_dir_timer = 0.0f; // 改变方向的倒计时
        float last_frame_time = 0.0f; // 上一帧的时间点
        std::mt19937 random_engine{std::random_device{}()}; // 随机数生成器

        auto window_init() -> void;

        // 10个立方体位置信息
        const std::vector<glm::vec3> cube_positions = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                                       glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                                       glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                                       glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                                       glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

        // 绘制立方体所用的顶点数据 (36个顶点)
        const std::vector<float> vertices = {
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
    };


    Window::Window(const std::string_view &title, const int width, const int height) :
        window_title(title), window_width(width), window_height(height) {
        window_init();

        shader_program = std::make_shared<Shader>("./res/shader/first_vert.glsl", "./res/shader/first_frag.glsl");
        shader_program->use();

        // 设置顶点数组对象和顶点缓冲对象
        glGenVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);

        glGenBuffers(1, &vertex_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // 设置顶点属性指针
        // 位置属性: 0, 3个float, 步长 5 * sizeof(float), 偏移 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);

        // 纹理坐标属性: 1, 2个float, 步长 5 * sizeof(float), 偏移 3 * sizeof(float)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        if (const auto img_data = load_image("./res/image/oak_planks.png"); img_data.data != nullptr) {
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (const auto img_data = load_image("./res/image/diamond_pickaxe.png"); img_data.data != nullptr) {
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

        shader_program->set_int("texture1", 0);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_2);

        shader_program->use();

        auto model = glm::mat4(1.0f);
        auto view = glm::mat4(1.0f);
        auto projection = glm::mat4(1.0f);

        const auto current_ticks = SDL_GetTicks();
        const auto delta_time = (current_ticks - last_frame_time) / 1000.0f;

        // model = glm::rotate(model, delta_time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection =
                glm::perspective(glm::radians(45.0f),
                                 static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

        auto model_loc = glGetUniformLocation(shader_program->get_id(), "model");
        auto view_loc = glGetUniformLocation(shader_program->get_id(), "view");

        // glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        shader_program->set_mat4("projection", projection);

        glBindVertexArray(vertex_array_object);

        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            // 让每个箱子以不同的速度和轴向旋转
            model = glm::rotate(model, glm::radians(angle) + current_ticks / 1000.0f * glm::radians(50.0f),
                                glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


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

        // 启用深度测试
        glEnable(GL_DEPTH_TEST);

        // 设置初始视口大小
        glViewport(0, 0, window_width, window_height);

        SDL_GL_SetSwapInterval(1); // Enable VSyn c
    }
} // namespace first_opengl
