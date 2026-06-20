#pragma once

// Inclusiones de OpenGL || OpenGL includes
#define GLEW_STATIC
#include <GL/glew.h>

// Gestor de shaders || Shader manager
#include "Shader.h"

class Bloom {
public:
    Bloom();
    ~Bloom();

    // Inicializar framebuffers y shaders || Initialize framebuffers and shaders
    void Init(int width, int height);

    // Renderizar post-procesado bloom || Render bloom post-processing
    void Render(bool enabled);

    // Recursos accesibles desde el loop || Resources accessible from the loop
    GLuint hdrFBO;
    GLuint colorBuffer;
    unsigned int rboDepth;

    int m_width;
    int m_height;

private:
    // Identificadores para la tecnica de ping pong || Identifiers for the ping pong technique
    GLuint pingpongFBO[2];
    GLuint pingpongColor[2];

    // Datos del cuadrante para renderizado en pantalla || Quad data for screen space rendering
    unsigned int quadVAO, quadVBO;

    // Coleccion de shaders para efectos post-proceso || Shaders collection for post-process effects
    Shader* brightExtractShader;
    Shader* blurShader;
    Shader* bloomFinalShader;

    // Dibujar el cuadrante alineado a la pantalla || Draw the screen aligned quad
    void RenderQuad();
};



