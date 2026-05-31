#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

// =========================
// MATRICES
// =========================
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// =========================
// OPCIÓN DEBUG SKYBOX
// =========================
uniform bool useViewNoTranslation; // false por defecto

void main()
{
    // POSICIÓN EN MUNDO
    FragPos = vec3(model * vec4(aPos, 1.0));

    // NORMAL EN MUNDO
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // UVs
    TexCoord = aTexCoord;

    // OUTPUT FINAL
    if (useViewNoTranslation) {
        // Elimina traslación de la vista (modo skybox)
        mat4 viewNoTranslation = mat4(mat3(view));
        gl_Position = projection * viewNoTranslation * model * vec4(aPos, 1.0);
    } else {
        // Modo normal
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}