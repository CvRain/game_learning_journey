//
// Created by cvrain on 2026/1/5.
//
module;

#include <glad/glad.h>
#include <stdexcept>
#include <string>

export module first_opengl.shader;

import first_opengl.file_operation;

export class Shader {
public:
    explicit Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader() = default;
    auto get_id() const -> unsigned int { return id; }
    auto use() const -> void;
    auto set_bool(const std::string &name, bool value) const -> void;
    auto set_int(const std::string &name, int value) const -> void;
    auto set_float(const std::string &name, float value) const -> void;

private:
    unsigned int id{};
};

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    const auto vertexCode = first_opengl::read_source_code(vertexPath);
    const auto fragmentCode = first_opengl::read_source_code(fragmentPath);

    const auto &vertexShader = first_opengl::shader_compiler(vertexCode, GL_VERTEX_SHADER);
    const auto &fragmentShader = first_opengl::shader_compiler(fragmentCode, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::string errorMessage = "Shader program linking failed: ";
        errorMessage += infoLog;
        throw std::runtime_error(errorMessage);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

auto Shader::use() const -> void { glUseProgram(id); }

auto Shader::set_bool(const std::string &name, const bool value) const -> void {
    glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

auto Shader::set_int(const std::string &name, const int value) const -> void {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

auto Shader::set_float(const std::string &name, const float value) const -> void {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}