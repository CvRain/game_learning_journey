#version 330 core

out vec4 FragColor; // 输出到帧缓冲的最终颜色

uniform vec4 ourColor;

void main()
{
    FragColor = ourColor;
}