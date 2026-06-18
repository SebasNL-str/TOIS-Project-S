#include "Bloom.h"
#include <iostream>

Bloom::Bloom() : quadVAO(0), quadVBO(0),
brightExtractShader(nullptr),
blurShader(nullptr),
bloomFinalShader(nullptr),
hdrFBO(0),
colorBuffer(0) {
    pingpongFBO[0] = 0; pingpongFBO[1] = 0;
    pingpongColor[0] = 0; pingpongColor[1] = 0;
}

Bloom::~Bloom() {
    glDeleteFramebuffers(1, &hdrFBO);
    glDeleteTextures(1, &colorBuffer);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColor);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    delete brightExtractShader;
    delete blurShader;
    delete bloomFinalShader;
}

void Bloom::Init(int width, int height) {
    // 1. Configurar HDR FBO
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Crear textura de color flotante (HDR)
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Evita artefactos en los bordes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    // CREAR Y ACOPLAR EL RENDERBUFFER DE PROFUNDIDAD
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Error: HDR framebuffer no está completo!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Configurar Ping-pong FBOs para el Desenfoque (Blur)
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColor);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColor[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColor[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Error: PingPong FBO " << i << " no está completo!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Cargar shaders
    brightExtractShader = new Shader("resources/Shaders/quad.vert", "resources/Shaders/brightExtract.frag");
    blurShader = new Shader("resources/Shaders/quad.vert", "resources/Shaders/blur.frag");
    bloomFinalShader = new Shader("resources/Shaders/quad.vert", "resources/Shaders/bloomFinal.frag");
}

void Bloom::Render(bool enabled) {
    // -------------------------------------------------------------------------
    // PASO 1: Extraer las zonas brillantes de la escena HDR
    // -------------------------------------------------------------------------
    brightExtractShader->Use();
    // Extraemos el brillo en el FBO 0. Su textura ligada es pingpongColor[0]
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    RenderQuad();

    // -------------------------------------------------------------------------
    // PASO 2: Aplicar el desenfoque Gaussiano de dos pases (Ping-Pong) CORREGIDO
    // -------------------------------------------------------------------------
    blurShader->Use();
    bool horizontal = true;
    int blurPasses = 10;

    for (int i = 0; i < blurPasses; i++) {
        // Vinculamos el FBO en el que vamos a ESCRIBIR (Alterna entre 1 y 0)
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blurShader->SetInt("horizontal", horizontal);

        glActiveTexture(GL_TEXTURE0);
        if (i == 0) {
            // La mismísima primera pasada lee del FBO 0 (donde extrajimos el brillo puro)
            glBindTexture(GL_TEXTURE_2D, pingpongColor[0]);
        }
        else {
            // Las demás pasadas leen del FBO opuesto al que estamos escribiendo actualemente
            glBindTexture(GL_TEXTURE_2D, pingpongColor[!horizontal]);
        }

        RenderQuad();
        horizontal = !horizontal; // Alternar dirección
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // -------------------------------------------------------------------------
    // PASO 3: Combinar de forma aditiva la Escena HDR + El Brillo Borroso
    // -------------------------------------------------------------------------
    bloomFinalShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    bloomFinalShader->SetInt("sceneTex", 0);

    // Al hacer exactamente 10 pasadas (número par), el último renderizado estable
    // del blur matemático real queda almacenado en pingpongColor[0].
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColor[0]);
    bloomFinalShader->SetInt("blurTex", 1);

    bloomFinalShader->SetBool("bloomEnabled", enabled);

    RenderQuad();
}

void Bloom::RenderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            // posiciones   // coordenadas de textura
            -1.0f,  1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f, -1.0f,   1.0f, 0.0f,

            -1.0f,  1.0f,   0.0f, 1.0f,
             1.0f, -1.0f,   1.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}



