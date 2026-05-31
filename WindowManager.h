#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

// Inicializa GLFW, calcula el tamaño de pantalla completa y crea la ventana
GLFWwindow* InitWindow(int& widthR, int& heightR, const std::string& title);

// Carga el icono de la ventana usando tu configuración de SOIL
void LoadWindowIcon(GLFWwindow* window, const std::string& path);

// Configura GLEW y los estados nativos de OpenGL (Depth Test, Blending)
bool InitOpenGLState();