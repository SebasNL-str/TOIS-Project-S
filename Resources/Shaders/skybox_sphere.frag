#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D skyboxSphere;

void main()
{
    FragColor = texture(skyboxSphere, TexCoords);
}