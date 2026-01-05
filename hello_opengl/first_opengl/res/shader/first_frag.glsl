#version 330 core

in vec4 vertexColor;
out vec4 FragColor; // 输出到帧缓冲的最终颜色

void main()
{
    FragColor = vertexColor;
}