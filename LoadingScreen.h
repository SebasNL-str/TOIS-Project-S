#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/constants.hpp>
#include "Shader.h" // Incluye tu propia clase de Shaders
// =========================
// Funciones de barra
// =========================
void InitProgressBar(GLuint& barVAO, GLuint& barVBO);
void DrawProgressBar(GLuint barVAO, GLuint barVBO, Shader& loadingShader, float progress);

// Nueva función para dibujar el spinner/rueda de carga
void DrawLoadingSpinner(Shader& loadingShader, float time);

void UpdateLoadingScreen(GLFWwindow* window, GLuint barVAO, GLuint barVBO, Shader& loadingShader, int loadedAssets, int totalAssets);