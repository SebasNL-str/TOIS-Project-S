#version 330 core

// Atributos de entrada del vertice || Vertex input attributes
layout(location = 0) in vec3 aPos;

// Matrices de transformacion || Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Coordenadas espaciales de salida || Output spatial coordinates
out vec3 WorldPos;

void main()
{
    // Calcular la posicion del vertice en el espacio del mundo || Calculate vertex position in world space
    vec4 world = model * vec4(aPos, 1.0);
    WorldPos = world.xyz;
    
    // Calcular la posicion proyectada final en pantalla || Calculate final projected screen position
    gl_Position = projection * view * world;
}

