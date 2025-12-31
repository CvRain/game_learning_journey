module;

#include <glad/glad.h>
#include <array>

export module first_opengl.renderer;

import first_opengl.shader;
import first_opengl.mesh;

/**
 * Renderer 类 - 管理渲染逻辑
 */
export class Renderer {
public:
    Renderer() = default;

    /**
     * 清除屏幕
     * @param r 红色分量
     * @param g 绿色分量
     * @param b 蓝色分量
     * @param a 透明度分量
     */
    static void clear(float r = 0.1f, float g = 0.2f, float b = 0.3f, float a = 1.0f);

    /**
     * 渲染网格
     * @param shader 使用的着色器
     * @param mesh 要渲染的网格
     */
    static void draw(const Shader &shader, const Mesh &mesh);

    /**
     * 设置清除颜色
     */
    static void set_clear_color(float r, float g, float b, float a = 1.0f);

private:
    inline static std::array<float, 4> clear_color{0.1f, 0.2f, 0.3f, 1.0f};
};

void Renderer::clear(const float r, const float g, const float b, const float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(const Shader &shader, const Mesh &mesh) {
    shader.use();
    mesh.draw();
}

void Renderer::set_clear_color(const float r, const float g, const float b, const float a) {
    clear_color = {r, g, b, a};
    glClearColor(r, g, b, a);
}

