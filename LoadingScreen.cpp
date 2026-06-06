#include "LoadingScreen.h"

void InitProgressBar(GLuint& barVAO, GLuint& barVBO) {
    glGenVertexArrays(1, &barVAO);
    glGenBuffers(1, &barVBO);
    glBindVertexArray(barVAO);
    glBindBuffer(GL_ARRAY_BUFFER, barVBO);

    float barVertices[] = {
        -0.4f, -0.8f,
        -0.4f, -0.75f,
         0.4f, -0.75f,
         0.4f, -0.8f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(barVertices), barVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void DrawProgressBar(GLuint barVAO, GLuint barVBO, Shader& loadingShader, float progress) {
    float barWidth = 0.8f;
    float barHeight = 0.05f;

    if (progress > 1.0f) progress = 1.0f;
    if (progress < 0.0f) progress = 0.0f;

    // 1. FONDO GRIS
    float backgroundVertices[] = {
        -barWidth / 2.0f, -0.8f,
        -barWidth / 2.0f, -0.8f + barHeight,
         barWidth / 2.0f, -0.8f + barHeight,
         barWidth / 2.0f, -0.8f
    };
    glBindBuffer(GL_ARRAY_BUFFER, barVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(backgroundVertices), backgroundVertices);
    loadingShader.Use();
    loadingShader.SetVec3("barColor", glm::vec3(0.2f, 0.2f, 0.2f));
    glBindVertexArray(barVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // 2. RELLENO VERDE
    float filled = barWidth * progress;
    float startX = -barWidth / 2.0f;
    float barVertices[] = {
        startX,          -0.8f,
        startX,          -0.8f + barHeight,
        startX + filled, -0.8f + barHeight,
        startX + filled, -0.8f
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(barVertices), barVertices);
    loadingShader.SetVec3("barColor", glm::vec3(0.2f, 0.8f, 0.2f));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// NUEVA FUNCIÓN: Dibuja un círculo giratorio de puntos arqueados
void DrawLoadingSpinner(Shader& loadingShader, float time) {
    const int NUM_DOTS = 8;
    float radius = 0.05f;           // Tamaño del anillo contenedor
    glm::vec2 center(0.0f, -0.6f);  // Ubicación arriba de la barra de progreso

    glEnable(GL_PROGRAM_POINT_SIZE);

    // 1. Posiciones FIJAS para los círculos (No rotan)
    float spinnerVertices[NUM_DOTS * 2];
    for (int i = 0; i < NUM_DOTS; i++) {
        float angle = i * (2.0f * glm::pi<float>() / NUM_DOTS);
        spinnerVertices[i * 2] = center.x + cos(angle) * radius; // X fija
        spinnerVertices[i * 2 + 1] = center.y + sin(angle) * radius; // Y fija
    }

    // Enviar las posiciones fijas a buffers temporales
    GLuint spinnerVAO, spinnerVBO;
    glGenVertexArrays(1, &spinnerVAO);
    glGenBuffers(1, &spinnerVBO);

    glBindVertexArray(spinnerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spinnerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spinnerVertices), spinnerVertices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    loadingShader.Use();
    glPointSize(10.0f); // Círculos un poco más grandes para que se note el efecto

    // 2. Calcular cuál círculo debe estar "encendido" en este milisegundo
    float speed = 6.0f; // Velocidad de la secuencia (número de pasos por segundo)

    for (int i = 0; i < NUM_DOTS; i++) {
        // MATEMÁTICAS DEL DESVANECIMIENTO:
        // Calculamos la distancia de fase entre el tiempo actual y la posición de este círculo.
        // Esto genera un valor que va disminuyendo suavemente (estela) desde el punto activo.
        float phase = fmod(time * speed - i, (float)NUM_DOTS);
        if (phase < 0.0f) phase += NUM_DOTS; // Corrección para ciclos limpios

        // Factor de brillo: 1.0 es encendido máximo, 0.1 es apagado/tenue
        float brightness = 1.0f - (phase / (float)NUM_DOTS);
        if (brightness < 0.1f) brightness = 0.1f; // Que nunca se borre del todo el fondo

        // Aplicamos el brillo al color verde original (0.2, 0.8, 0.2)
        glm::vec3 finalColor = glm::vec3(0.2f * brightness, 0.8f * brightness, 0.2f * brightness);

        loadingShader.SetVec3("barColor", finalColor);
        glDrawArrays(GL_POINTS, i, 1);
    }

    // Limpieza de memoria
    glDeleteBuffers(1, &spinnerVBO);
    glDeleteVertexArrays(1, &spinnerVAO);
}

void UpdateLoadingScreen(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, int loadedAssets, int totalAssets) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    float progress = (totalAssets > 0) ? (float)loadedAssets / (float)totalAssets : 0.0f;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // 1. Dibujar la barra normal
    DrawProgressBar(barVAO, barVBO, loadingShader, progress);

    // 2. Dibujar el Spinner usando el tiempo transcurrido de GLFW
    float currentTime = static_cast<float>(glfwGetTime());
    DrawLoadingSpinner(loadingShader, currentTime);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glfwSwapBuffers(window);
    glfwPollEvents();
}