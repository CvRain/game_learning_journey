module;

#include <glad/glad.h>
#include <vector>
#include <span>

export module first_opengl.mesh;

/**
 * Mesh 类 - 管理顶点数据和 OpenGL 缓冲对象
 * 使用 RAII 管理 VAO 和 VBO 资源
 */
export class Mesh {
public:
    /**
     * 创建一个网格
     * @param vertices 顶点数据
     */
    explicit Mesh(std::span<const float> vertices);

    ~Mesh();

    // 禁止拷贝，允许移动
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;

    /**
     * 绘制网格
     */
    void draw() const;

    /**
     * 获取顶点数量
     */
    [[nodiscard]] auto get_vertex_count() const -> size_t {
        return vertex_count;
    }

private:
    GLuint VAO{0};
    GLuint VBO{0};
    size_t vertex_count{0};

    void setup_mesh(std::span<const float> vertices);
};

Mesh::Mesh(std::span<const float> vertices) {
    vertex_count = vertices.size() / 3; // 假设每个顶点 3 个分量 (x, y, z)
    setup_mesh(vertices);
}

Mesh::~Mesh() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
}

Mesh::Mesh(Mesh &&other) noexcept
    : VAO(other.VAO), VBO(other.VBO), vertex_count(other.vertex_count) {
    other.VAO = 0;
    other.VBO = 0;
    other.vertex_count = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    if (this != &other) {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
        }

        VAO = other.VAO;
        VBO = other.VBO;
        vertex_count = other.vertex_count;

        other.VAO = 0;
        other.VBO = 0;
        other.vertex_count = 0;
    }
    return *this;
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count));
    glBindVertexArray(0);
}

void Mesh::setup_mesh(std::span<const float> vertices) {
    // 生成并绑定 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

