#version 330 core
// Atributos de entrada de posicion y coordenadas UV || Position and UV coordinate input attributes
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

// Coordenadas de textura de salida || Output texture coordinates
out vec2 TexCoords;

void main()
{
    // Pasar las coordenadas de textura al fragment shader || Pass texture coordinates to the fragment shader
    TexCoords = aTexCoords;
    
    // Proyectar la geometria bidimensional directamente en pantalla || Project two dimensional geometry directly onto the screen
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
