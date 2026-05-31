#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    // Quitar traslación de la vista
    mat4 viewNoTranslation = mat4(mat3(view));
    gl_Position = projection * viewNoTranslation * vec4(aPos, 1.0);
}
