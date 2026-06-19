#version 330 core
// Atributos de entrada del vertice || Vertex input attributes
layout (location = 0) in vec3 aPos;

// Coordenadas de textura de salida || Output texture coordinates
out vec3 TexCoords;

// Matrices de transformacion || Transformation matrices
uniform mat4 projection;
uniform mat4 view;

void main()
{
    // Asignar los vectores de posicion como coordenadas cubicas || Assign position vectors as cube map coordinates
    TexCoords = aPos;
    
    // Eliminar traslacion de la matriz de vista para el fondo || Remove translation from view matrix for background
    mat4 viewNoTranslation = mat4(mat3(view));
    
    // Calcular posicion proyectada final del skybox || Calculate final projected position of the skybox
    gl_Position = projection * viewNoTranslation * vec4(aPos, 1.0);
}
