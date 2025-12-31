module;
#include <string_view>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <glad/glad.h>

export module first_opengl.file_operation;

/**
 * 用于读取glsl shader代码文件
 * @param file_path 接受一个文件路径
 * @return 返回此文件的源代码
 */
export auto read_source_code(const std::string_view &file_path) -> std::string {
    std::ifstream file_stream{std::filesystem::path(file_path)};
    if (!file_stream.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(file_path));
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

export auto shader_compiler(const std::string_view &source_code, const GLenum shader_type) {
    const auto shader = glCreateShader(shader_type);
    const auto source_cstr = source_code.data();
    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    // 检查编译状态
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::string error_message = "Shader compilation failed: ";
        error_message += info_log;
        throw std::runtime_error(error_message);
    }

    return shader;
}
