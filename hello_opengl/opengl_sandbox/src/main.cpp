#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/Camera.h"
#include "core/Shader.h"

#include <iostream>

// 设置屏幕宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    // 初始化 SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 设置 OpenGL 版本 (3.3 Core Profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // 创建窗口
    SDL_Window *window =
            SDL_CreateWindow("OpenGL Sandbox", SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 创建 OpenGL 上下文
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cout << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 开启 VSync (可选)
    // SDL_GL_SetSwapInterval(1);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 编译着色器
    // 注意：这里的路径是相对于可执行文件的，或者相对于 CMake 设置的资源目录
    // CMake 中我们将 res 复制到了构建目录
    Shader ourShader("res/shaders/test.vert", "res/shaders/test.frag");

    // 设置顶点数据
    float vertices[] = {-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, 1.0f,
                        -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

                        -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 0.0f,
                        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
                        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,

                        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
                        0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

                        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

                        -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
                        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
                        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.0f};

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 设置SDL相对鼠标模式 (FPS视角)
    SDL_SetWindowRelativeMouseMode(window, true);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // 计算 Delta Time
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理事件
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    quit = true;
                }
            }
            if (e.type == SDL_EVENT_MOUSE_MOTION) {
                // SDL3 鼠标移动事件: e.motion.xrel, e.motion.yrel
                camera.ProcessMouseMovement(e.motion.xrel, -e.motion.yrel);
            }
            if (e.type == SDL_EVENT_MOUSE_WHEEL) {
                camera.ProcessMouseScroll(e.wheel.y);
            }
        }

        // 键盘连续输入
        const bool *currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_W])
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (currentKeyStates[SDL_SCANCODE_S])
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (currentKeyStates[SDL_SCANCODE_A])
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (currentKeyStates[SDL_SCANCODE_D])
            camera.ProcessKeyboard(RIGHT, deltaTime);

        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活 shader
        ourShader.use();

        // 变换矩阵
        glm::mat4 projection =
                glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)SDL_GetTicks() / 1000.0f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 交换缓冲
        SDL_GL_SwapWindow(window);
    }

    // 清理
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
