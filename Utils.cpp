#include "Utils.h"

#include <fstream>

// Comprobar la existencia fisica de un archivo || Check the physical existence of a file
bool FileExists(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    return file.good();
}

// Calcular y actualizar el contador de FPS en la ventana || Calculate and update FPS counter on the window
void updateFPS(GLFWwindow* window) {
    static double previousTime = glfwGetTime();
    static int frameCount = 0;

    double currentTime = glfwGetTime();
    frameCount++;

    // Verificar si transcurrio un segundo completo || Check if a full second has passed
    if (currentTime - previousTime >= 1.0) {
        double fps = double(frameCount) / (currentTime - previousTime);

        // Actualizar la barra de titulo con el rendimiento || Update title bar with performance data
        std::string windowTitle = "Proyecto Cementerio 3D | FPS: " + std::to_string(int(fps));
        glfwSetWindowTitle(window, windowTitle.c_str());

        // Reiniciar variables de control de tiempo || Reset time control variables
        frameCount = 0;
        previousTime = currentTime;
    }
}
