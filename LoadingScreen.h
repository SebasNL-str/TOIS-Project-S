#pragma once
// GL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/constants.hpp>

// Project include
#include "Shader.h"

struct LoadingFadeSettings
{
    float durationSeconds = 0.75f;
    float frameStepSeconds = 0.016f;
};

// Inicializacion de la barra de progreso || Progress bar initialization
void InitProgressBar(GLuint& barVAO, GLuint& barVBO);
void DrawProgressBar(GLuint barVAO, GLuint barVBO, Shader& loadingShader, float progress);

// Nueva función para dibujar el spinner/rueda de carga || Function to draw a loading spinning wheel
void DrawLoadingSpinner(Shader& loadingShader, float time);

// Actualiza los estados de la pantalla de carga || Updates the loading screen status
void UpdateLoadingScreen(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, int loadedAssets, int totalAssets);
void PlayLoadingFadeOut(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, const LoadingFadeSettings& settings = LoadingFadeSettings());
