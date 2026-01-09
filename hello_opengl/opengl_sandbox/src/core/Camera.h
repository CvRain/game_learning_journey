#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// 定义摄像机移动的几个可能选项
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// 默认摄像机值
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    // 摄像机属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // 欧拉角
    float Yaw;
    float Pitch;
    // 摄像机选项
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 构造函数 - 向量
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH);
    // 构造函数 - 标量
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // 返回视图矩阵
    glm::mat4 GetViewMatrix();

    // 处理键盘输入
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // 处理鼠标移动
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // 处理滚轮
    void ProcessMouseScroll(float yoffset);

private:
    // 根据摄像机的（更新后的）欧拉角计算 Front 向量
    void updateCameraVectors();
};
