#include "Utils.h"
#include <fstream>

bool FileExists(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    return file.good();
}


void updateFPS(GLFWwindow* window) {
    // Variables estáticas para que conserven su valor entre llamadas
    static double previousTime = glfwGetTime();
    static int frameCount = 0;

    double currentTime = glfwGetTime();
    frameCount++;

    // Si ha pasado 1 segundo o más, actualizamos el título
    if (currentTime - previousTime >= 1.0) {
        // Calculamos los FPS reales basados en el tiempo exacto transcurrido
        double fps = double(frameCount) / (currentTime - previousTime);

        // Creamos el nuevo título de la ventana
        std::string windowTitle = "Proyecto Cementerio 3D | FPS: " + std::to_string(int(fps));
        glfwSetWindowTitle(window, windowTitle.c_str());

        // Reiniciamos el contador y el temporizador
        frameCount = 0;
        previousTime = currentTime;
    }
}