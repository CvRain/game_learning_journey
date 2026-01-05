#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为1

out vec4 vertexColor; // 为片段着色器指定一个颜色输出

uniform vec3 shape_offset = vec3(0.0, 0.0, 0.0);

void main()
{
    gl_Position = vec4(aPos + shape_offset, 1.0); // 注意我们如何把一个vec3作为vec4的构造器的参数
    vertexColor = vec4(aColor, 1.0); // 将我们的输出变量设置为从顶点数据传来的输入变量
}