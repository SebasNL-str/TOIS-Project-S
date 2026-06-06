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

void DrawLoadingSpinner(Shader& loadingShader, float time) {
    const int NUM_DOTS = 8;
    float radius = 0.05f;
    glm::vec2 center(0.0f, -0.6f);

    glEnable(GL_PROGRAM_POINT_SIZE);

    float spinnerVertices[NUM_DOTS * 2];
    for (int i = 0; i < NUM_DOTS; i++) {
        float angle = i * (2.0f * glm::pi<float>() / NUM_DOTS);
        spinnerVertices[i * 2] = center.x + cos(angle) * radius;
        spinnerVertices[i * 2 + 1] = center.y + sin(angle) * radius;
    }

    GLuint spinnerVAO, spinnerVBO;
    glGenVertexArrays(1, &spinnerVAO);
    glGenBuffers(1, &spinnerVBO);

    glBindVertexArray(spinnerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spinnerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spinnerVertices), spinnerVertices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    loadingShader.Use();
    glPointSize(14.0f); // Un tamaño óptimo para apreciar la ola circular

    // Configuración de Uniforms para la GPU
    loadingShader.SetBool("isSpinner", true);
    loadingShader.SetFloat("globalTime", time); // Pasamos el tiempo absoluto real sin modificaciones
    loadingShader.SetVec3("barColor", glm::vec3(0.2f, 0.8f, 0.2f)); // Verde TOIS

    // Dibujamos todos los círculos de un solo golpe con una única llamada de dibujo
    glDrawArrays(GL_POINTS, 0, NUM_DOTS);

    // Desactivamos el modo spinner para el siguiente frame de la barra
    loadingShader.SetBool("isSpinner", false);

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