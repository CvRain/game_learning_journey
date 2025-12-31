module;
#include <string_view>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdexcept>

export module first_opengl.file_operation;

/**
 * 读取文本文件内容
 * @param file_path 文件路径
 * @return 返回文件的内容
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

