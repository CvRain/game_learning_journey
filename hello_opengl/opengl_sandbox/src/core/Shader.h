#pragma once

#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
    unsigned int ID;

    // 构造函数读取并构建着色器
    Shader(const char *vertexPath, const char *fragmentPath);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
