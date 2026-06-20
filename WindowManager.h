#pragma once
// GL include
#define GLEW_STATIC
#include <GL/glew.h>

// std include
#include <string>

// GLFW include
#include <GLFW/glfw3.h>

// Inicializar la ventana de GLFW y calcular el tamaño de pantalla completa || Initialize GLFW window and calculate full screen size
GLFWwindow* InitWindow(int& widthR, int& heightR, const std::string& title);

// Cargar el icono del sistema operativo en la ventana || Load the operating system icon into the window
void LoadWindowIcon(GLFWwindow* window, const std::string& path);

// Configurar extensiones de GLEW y estados nativos de OpenGL || Configure GLEW extensions and native OpenGL states
bool InitOpenGLState();