#include "LoadingScreen.h"

namespace
{
    void UpdateLoadingScreenWithOpacity(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, int loadedAssets, int totalAssets, float opacity);
}

// Inicializar los buffers para la barra de progreso || Initialize progress bar buffers
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

// Dibujar los componentes de la barra de progreso || Draw progress bar components
void DrawProgressBar(GLuint barVAO, GLuint barVBO, Shader& loadingShader, float progress) {
    float barWidth = 0.8f;
    float barHeight = 0.05f;

    // Limitar el rango del progreso || Clamp progress range
    if (progress > 1.0f) progress = 1.0f;
    if (progress < 0.0f) progress = 0.0f;

    // Actualizar vertices y dibujar el fondo gris || Update vertices and draw grey background
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

    // Actualizar vertices y dibujar el relleno beige || Update vertices and draw beige filling
    float filled = barWidth * progress;
    float startX = -barWidth / 2.0f;
    float barVertices[] = {
        startX,          -0.8f,
        startX,          -0.8f + barHeight,
        startX + filled, -0.8f + barHeight,
        startX + filled, -0.8f
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(barVertices), barVertices);
    loadingShader.SetVec3("barColor", glm::vec3(0.95f, 0.87f, 0.73f));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// Renderizar el indicador circular animado || Render the animated circular spinner
void DrawLoadingSpinner(Shader& loadingShader, float time) {
    const int NUM_DOTS = 16;

    float radius = 0.30f;
    glm::vec2 center(0.0f, 0.0f);

    // Obtener dimensiones actuales del viewport || Get current viewport dimensions
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float width = static_cast<float>(viewport[2]);
    float height = static_cast<float>(viewport[3]);
    float aspectRatio = (height > 0.0f) ? (width / height) : 1.0f;

    glEnable(GL_PROGRAM_POINT_SIZE);

    float spinnerVertices[(NUM_DOTS + 1) * 2];

    // Calcular posiciones de los puntos perimetrales || Calculate perimeter dots positions
    for (int i = 0; i < NUM_DOTS; i++) {
        float angle = i * (2.0f * glm::pi<float>() / NUM_DOTS);
        spinnerVertices[i * 2] = center.x + (cos(angle) * radius) / aspectRatio;
        spinnerVertices[i * 2 + 1] = center.y + sin(angle) * radius;
    }

    // Asignar coordenadas para el punto del centro || Assign coordinates for the center dot
    spinnerVertices[NUM_DOTS * 2] = 0.0f;
    spinnerVertices[NUM_DOTS * 2 + 1] = 0.0f;

    // Crear y cargar buffers locales temporales || Create and load temporary local buffers
    GLuint spinnerVAO, spinnerVBO;
    glGenVertexArrays(1, &spinnerVAO);
    glGenBuffers(1, &spinnerVBO);

    glBindVertexArray(spinnerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spinnerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spinnerVertices), spinnerVertices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Enviar variables uniformes al shader || Send uniform variables to the shader
    loadingShader.Use();
    loadingShader.SetBool("isSpinner", true);
    loadingShader.SetFloat("globalTime", time);
    loadingShader.SetVec3("barColor", glm::vec3(0.95f, 0.87f, 0.73f));
    loadingShader.SetVec3("screenSize", glm::vec3(width, height, aspectRatio));

    // Dibujar anillo exterior de puntos || Draw external dots ring
    glPointSize(18.0f);
    glDrawArrays(GL_POINTS, 0, NUM_DOTS);

    // Dibujar el punto central gigante || Draw the giant central dot
    glPointSize(160.0f);
    glDrawArrays(GL_POINTS, NUM_DOTS, 1);

    // Desactivar bandera y liberar recursos de memoria || Disable flag and release memory resources
    loadingShader.SetBool("isSpinner", false);
    glDeleteBuffers(1, &spinnerVBO);
    glDeleteVertexArrays(1, &spinnerVAO);
}

void UpdateLoadingScreen(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, int loadedAssets, int totalAssets) {
    UpdateLoadingScreenWithOpacity(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets, 1.0f);
}

void PlayLoadingFadeOut(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, const LoadingFadeSettings& settings) {
    if (settings.durationSeconds <= 0.0f) {
        UpdateLoadingScreenWithOpacity(window, barVAO, barVBO, loadingShader, 1, 1, 0.0f);
        return;
    }

    float startTime = static_cast<float>(glfwGetTime());
    float elapsed = 0.0f;

    while (elapsed < settings.durationSeconds && !glfwWindowShouldClose(window)) {
        elapsed = static_cast<float>(glfwGetTime()) - startTime;
        float fadeProgress = elapsed / settings.durationSeconds;
        if (fadeProgress > 1.0f) fadeProgress = 1.0f;
        if (fadeProgress < 0.0f) fadeProgress = 0.0f;

        float opacity = 1.0f - fadeProgress;
        UpdateLoadingScreenWithOpacity(window, barVAO, barVBO, loadingShader, 1, 1, opacity);
        glfwWaitEventsTimeout(settings.frameStepSeconds);
    }
}

// Actualizar el bucle de la ventana de carga || Update the loading window loop
namespace
{
void UpdateLoadingScreenWithOpacity(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, int loadedAssets, int totalAssets, float opacity) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Calcular la proporcion de la barra || Calculate bar ratio
    float progress = (totalAssets > 0) ? (float)loadedAssets / (float)totalAssets : 0.0f;

    // Limpiar buffers de color y profundidad || Clear color and depth buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Desactivar pruebas graficas 3D de fondo || Disable background 3D graphic tests
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (opacity > 1.0f) opacity = 1.0f;
    if (opacity < 0.0f) opacity = 0.0f;
    loadingShader.Use();
    loadingShader.SetFloat("loadingOpacity", opacity);

    // Renderizar barra y spinner decorativo || Render bar and decorative spinner
    DrawProgressBar(barVAO, barVBO, loadingShader, progress);
    float currentTime = static_cast<float>(glfwGetTime());
    DrawLoadingSpinner(loadingShader, currentTime);

    // Restaurar pruebas graficas y refrescar buffers || Restore graphic tests and swap buffers
    loadingShader.SetFloat("loadingOpacity", 1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSwapBuffers(window);
    glfwPollEvents();
}
}
