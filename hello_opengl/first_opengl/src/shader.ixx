module;

#include <glad/glad.h>
#include <string>
#include <string_view>
#include <format>
#include <stdexcept>

export module first_opengl.shader;

import first_opengl.file_operation;

/**
 * Shader 程序管理类
 * 使用 RAII 管理 OpenGL shader 资源
 */
export class Shader {
public:
    /**
     * 从文件加载并编译 shader 程序
     * @param vertex_path 顶点着色器路径
     * @param fragment_path 片段着色器路径
     */
    Shader(const std::string_view vertex_path, const std::string_view fragment_path);

    ~Shader();

    // 禁止拷贝，允许移动
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    /**
     * 使用此 shader 程序
     */
    void use() const;

    /**
     * 获取 shader 程序 ID
     */
    [[nodiscard]] auto get_id() const -> GLuint { return program_id; }

private:
    GLuint program_id{0};

    /**
     * 编译单个 shader
     */
    static auto compile_shader(const std::string &source, GLenum type) -> GLuint;

    /**
     * 链接 shader 程序
     */
    static auto link_program(GLuint vertex_shader, GLuint fragment_shader) -> GLuint;

    /**
     * 检查编译/链接错误
     */
    static void check_compile_errors(GLuint shader, const std::string &type);
};

Shader::Shader(const std::string_view vertex_path, const std::string_view fragment_path) {
    const auto vertex_code = read_source_code(vertex_path);
    const auto fragment_code = read_source_code(fragment_path);

    const auto vertex_shader = compile_shader(vertex_code, GL_VERTEX_SHADER);
    const auto fragment_shader = compile_shader(fragment_code, GL_FRAGMENT_SHADER);

    program_id = link_program(vertex_shader, fragment_shader);

    // 删除着色器，它们已经链接到程序中了
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader() {
    if (program_id != 0) {
        glDeleteProgram(program_id);
    }
}

Shader::Shader(Shader &&other) noexcept : program_id(other.program_id) {
    other.program_id = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
    if (this != &other) {
        if (program_id != 0) {
            glDeleteProgram(program_id);
        }
        program_id = other.program_id;
        other.program_id = 0;
    }
    return *this;
}

void Shader::use() const {
    glUseProgram(program_id);
}

auto Shader::compile_shader(const std::string &source, const GLenum type) -> GLuint {
    const auto shader = glCreateShader(type);
    const auto source_cstr = source.c_str();
    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    check_compile_errors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

    return shader;
}

auto Shader::link_program(const GLuint vertex_shader, const GLuint fragment_shader) -> GLuint {
    const auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    check_compile_errors(program, "PROGRAM");

    return program;
}

void Shader::check_compile_errors(const GLuint shader, const std::string &type) {
    GLint success;
    char info_log[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, info_log);
            throw std::runtime_error(
                std::format("Shader compilation error ({}): {}", type, info_log)
            );
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, info_log);
            throw std::runtime_error(
                std::format("Shader program linking error: {}", info_log)
            );
        }
    }
}

