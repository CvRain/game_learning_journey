module;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <sstream>
#include <string_view>


export module first_opengl.file_operation;

export struct ImageData {
    unsigned char *data;
    int width;
    int height;
    int channels;
};

export namespace first_opengl {
    /**
     * 用于读取glsl shader代码文件
     * @param file_path 接受一个文件路径
     * @return 返回此文件的源代码
     */
    auto read_source_code(const std::string_view &file_path) -> std::string {
        std::ifstream file_stream{std::filesystem::path(file_path)};
        if (!file_stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(file_path));
        }

        std::stringstream buffer;
        buffer << file_stream.rdbuf();
        return buffer.str();
    }

    auto shader_compiler(const std::string_view &source_code, const GLenum shader_type) {
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

    auto load_image(const std::string_view &file_path) -> ImageData {
        //检查 文件是否存在
        if (!std::filesystem::exists(file_path)) {
            throw std::runtime_error("File does not exist: " + std::string(file_path));
        }
        ImageData image_data{};
        image_data.data = stbi_load(file_path.data(), &image_data.width, &image_data.height, &image_data.channels, 0);
        if (!image_data.data) {
            throw std::runtime_error("Failed to load image: " + std::string(file_path));
        }
        return image_data;
    }

    auto get_image_format(const ImageData &image_data) -> GLenum {
        GLenum format = GL_RGB;
        if (image_data.channels == 1)
            format = GL_RED;
        else if (image_data.channels == 3)
            format = GL_RGB;
        else if (image_data.channels == 4)
            format = GL_RGBA;
        return format;
    }
} // namespace first_opengl
