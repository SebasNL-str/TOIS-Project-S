#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aTexCoords;
    mat4 viewNoTranslation = mat4(mat3(view));
    gl_Position = projection * viewNoTranslation * vec4(aPos, 1.0);
}