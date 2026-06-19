#version 330 core

// Atributos de entrada del vertice || Vertex input attributes
layout(location = 0) in vec3 aPos;

// Matrices de transformacion || Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calcular la posicion final proyectada del vertice || Calculate final projected vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
