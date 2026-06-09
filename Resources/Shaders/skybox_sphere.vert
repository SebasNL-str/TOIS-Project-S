#version 330 core
// Atributos de entrada del vertice || Vertex input attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

// Coordenadas de textura de salida || Output texture coordinates
out vec2 TexCoords;

// Matrices de transformacion || Transformation matrices
uniform mat4 projection;
uniform mat4 view;

void main()
{
    // Pasar coordenadas de textura || Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Eliminar traslacion de la matriz de vista para el fondo || Remove translation from view matrix for background
    mat4 viewNoTranslation = mat4(mat3(view));
    
    // Calcular posicion proyectada final del skybox || Calculate final projected position of the skybox
    gl_Position = projection * viewNoTranslation * vec4(aPos, 1.0);
}
