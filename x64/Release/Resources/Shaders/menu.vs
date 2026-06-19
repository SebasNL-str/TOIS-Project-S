#version 330 core
// Atributos de entrada del vertice || Vertex input attributes
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

// Variables de salida hacia el Fragment Shader || Output variables to Fragment Shader
out vec2 TexCoord;

void main()
{
    // Pasar coordenadas de textura || Pass texture coordinates
    TexCoord = aTexCoord;
    
    // Calcular la posicion final proyectada en pantalla || Calculate final projected screen position
    gl_Position = vec4(aPos, 0.0, 1.0);
}

