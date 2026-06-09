#pragma once

// std include
#include <vector>

// GL include
#include <GL/glew.h>

// Project include
#include "Shader.h"

// Estructura de datos para los vertices || Vertex data structure
struct Vertex {
    float x, y, z;     // Posicion || Position
    float nx, ny, nz;  // Normales || Normals
    float u, v;        // Coordenadas UV || UV Coordinates
};

// Clase para la gestion de mallas 3D || Class for 3D mesh management
class Mesh {
public:
    // Constructor de la malla con vertices e indices || Mesh constructor with vertices and indices
    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        GLuint diffuseTex,
        GLuint roughnessTex,
        GLuint metallicTex);

    // Renderizar la malla en pantalla || Render the mesh on screen
    void Draw();

private:
    // Objetos de buffer de OpenGL || OpenGL buffer objects
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;

    // Texturas del material || Material textures
    GLuint diffuseTex = 0;
    GLuint roughnessTex = 0;
    GLuint metallicTex = 0;

    // Almacenamiento de indices de la geometria || Storage of geometry indices
    std::vector<unsigned int> indices;
};