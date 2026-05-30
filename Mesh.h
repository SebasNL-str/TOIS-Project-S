#pragma once

#include <vector>
#include <GL/glew.h>
#include "Shader.h"

struct Vertex {
    float x, y, z;     // posición
    float nx, ny, nz;  // normales
    float u, v;        // coordenadas UV
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        GLuint diffuseTex,
        GLuint roughnessTex,
        GLuint metallicTex);

    void Draw();

private:
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;

    // Texturas
    GLuint diffuseTex = 0;
    GLuint roughnessTex = 0;
    GLuint metallicTex = 0;

    // Guardar índices
    std::vector<unsigned int> indices;
};