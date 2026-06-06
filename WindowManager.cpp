#include "WindowManager.h"
#include <iostream>
#include<SOIL2/SOIL2.h>

extern "C" unsigned char* SOIL_load_image(const char* filename, int* width, int* height, int* channels, int force_channels);

GLFWwindow* InitWindow(int& widthR, int& heightR, const std::string& title)
{
    if (!glfwInit())
    {
        std::cout << "Error inicializando GLFW\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);


    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    widthR = mode->width;
    heightR = mode->height;

    GLFWwindow* window = glfwCreateWindow(widthR, heightR, title.c_str(), NULL, NULL);
    if (!window)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return window;
}

void LoadWindowIcon(GLFWwindow* window, const std::string& path)
{
    int width, height, channels;
    // 4 equivale a SOIL_LOAD_RGBA
    unsigned char* pixels = SOIL_load_image(path.c_str(), &width, &height, &channels, 4);

    if (pixels) {
        GLFWimage images[1];
        images[0].width = width;
        images[0].height = height;
        images[0].pixels = pixels;

        glfwSetWindowIcon(window, 1, images);
        free(pixels);
    }
    else {
        std::cout << "Error cargando icono con SOIL\n";
    }
}

bool InitOpenGLState()
{
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error inicializando GLEW\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}