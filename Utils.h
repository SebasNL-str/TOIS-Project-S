#pragma once

// std include
#include <string>

// GLFW include
#include <GLFW/glfw3.h>

// Verifica si el archivo existe || Checks if file exists
bool FileExists(const std::string& path);

// Muestra los FPS || Shows the FPS
void updateFPS(GLFWwindow* window);