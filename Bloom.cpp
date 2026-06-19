#include "Bloom.h"

#include <iostream>

// Inicializacion de punteros y buffers a cero || Initialization of pointers and buffers to zero
Bloom::Bloom()
    : quadVAO(0)
    , quadVBO(0)
    , brightExtractShader(nullptr)
    , blurShader(nullptr)
    , bloomFinalShader(nullptr)
    , hdrFBO(0)
    , colorBuffer(0)
    , rboDepth(0)
{
    pingpongFBO[0] = 0;
    pingpongFBO[1] = 0;
    pingpongColor[0] = 0;
    pingpongColor[1] = 0;
}

// Liberacion de framebuffers, texturas y shaders de memoria || Deallocation of framebuffers, textures, and shaders from memory
Bloom::~Bloom()
{
    glDeleteFramebuffers(1, &hdrFBO);
    glDeleteTextures(1, &colorBuffer);
    glDeleteRenderbuffers(1, &rboDepth);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColor);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    delete brightExtractShader;
    delete blurShader;
    delete bloomFinalShader;
}

void Bloom::Init(int width, int height)
{
    // Guardar dimensiones internas de la pantalla || Save internal screen dimensions
    m_width = width;
    m_height = height;

    // Crear y vincular el FBO principal de HDR || Create and bind the main HDR FBO
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Configurar la textura flotante de color de 16-bits para HDR || Configure 16-bit floating-point color texture for HDR
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    // Generar y adjuntar el renderbuffer de profundidad y stencil || Generate and attach depth and stencil renderbuffer
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Verificar si el framebuffer HDR se creo correctamente || Check if the HDR framebuffer was created successfully
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Error: HDR framebuffer no esta completo!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Inicializar los FBOs y texturas para el ciclo alternado de desenfoque || Initialize FBOs and textures for the alternating blur loop
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColor);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColor[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColor[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Error: PingPong FBO " << i << " no esta completo!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Instanciar shaders especificos de post-procesado || Instantiate specific post-processing shaders
    brightExtractShader = new Shader("Resources/Shaders/quad.vert", "Resources/Shaders/brightExtract.frag");
    blurShader = new Shader("Resources/Shaders/quad.vert", "Resources/Shaders/blur.frag");
    bloomFinalShader = new Shader("Resources/Shaders/quad.vert", "Resources/Shaders/bloomFinal.frag");
}

void Bloom::Render(bool enabled)
{
    // Paso 1: Extraer los brillos de la escena en el primer buffer de ping-pong || Step 1: Extract bright areas into first ping-pong buffer
    brightExtractShader->Use();
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    RenderQuad();

    // Paso 2: Aplicar desenfoque de dos vias alternando buffers || Step 2: Apply two-way blur by alternating buffers
    blurShader->Use();
    bool horizontal = true;
    int blurPasses = 10;

    for (int i = 0; i < blurPasses; i++)
    {
        // Alternar FBO de destino segun la direccion del desenfoque || Alternate target FBO depending on blur direction
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glViewport(0, 0, m_width, m_height);
        blurShader->SetInt("horizontal", horizontal);

        // Seleccionar la textura origen correcta para el pase actual || Select correct source texture for current pass
        glActiveTexture(GL_TEXTURE0);
        if (i == 0)
            glBindTexture(GL_TEXTURE_2D, pingpongColor[0]);
        else
            glBindTexture(GL_TEXTURE_2D, pingpongColor[!horizontal]);

        RenderQuad();
        horizontal = !horizontal;
    }

    // Regresar al buffer de la pantalla por defecto || Return to default window framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);

    // Paso 3: Combinar la escena original y el mapa difuminado final || Step 3: Blend original scene and final blurred map
    bloomFinalShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    bloomFinalShader->SetInt("sceneTex", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColor[0]);
    bloomFinalShader->SetInt("blurTex", 1);

    bloomFinalShader->SetBool("bloomEnabled", enabled);

    RenderQuad();
}

void Bloom::RenderQuad()
{
    // Inicializar geometrias del quad si es la primera ejecucion || Initialize quad geometries if first run
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Atributo de posicion (X, Y, Z) || Position attribute (X, Y, Z)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // Atributo de coordenadas de textura (U, V) || Texture coordinates attribute (U, V)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    // Dibujar el quad usando una franja de triangulos || Draw quad using a triangle strip
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}