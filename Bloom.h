#pragma once

// GL includes
#define GLEW_STATIC
#include <GL/glew.h>

// Shader manager
#include "Shader.h"

class Bloom {
public:
    Bloom();
    ~Bloom();

    // Inicializar framebuffers y shaders
    void Init(int width, int height);

    // Renderizar post-procesado bloom
    void Render(bool enabled); // Ahora acepta el estado de debug

    // Recursos accesibles desde el loop
    GLuint hdrFBO;
    GLuint colorBuffer;
    unsigned int rboDepth;

    int m_width;  // <--- AGREGA ESTA VARIABLE
    int m_height; // <--- AGREGA ESTA VARIABLE

private:
    GLuint pingpongFBO[2];
    GLuint pingpongColor[2];

    unsigned int quadVAO, quadVBO;

    Shader* brightExtractShader;
    Shader* blurShader;
    Shader* bloomFinalShader;

    void RenderQuad();
};


