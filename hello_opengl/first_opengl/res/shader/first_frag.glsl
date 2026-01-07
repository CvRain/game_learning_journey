#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(ourTexture, TexCoord) * vec4(ourColor, 1.0), texture(texture2, TexCoord), 0.6);
}