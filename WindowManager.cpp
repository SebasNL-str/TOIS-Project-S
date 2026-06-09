#include "WindowManager.h"

#include <iostream>

#include<SOIL2/SOIL2.h>

// Declaracion externa de la funcion de carga de imagenes de SOIL || External declaration of SOIL image loading function
extern "C" unsigned char* SOIL_load_image(const char* filename, int* width, int* height, int* channels, int force_channels);

// Inicializar la ventana de la aplicacion con GLFW || Initialize the application window using GLFW
GLFWwindow* InitWindow(int& widthR, int& heightR, const std::string& title)
{
    // Validar inicializacion de GLFW || Validate GLFW initialization
    if (!glfwInit())
    {
        std::cout << "Error inicializando GLFW\n";
        return nullptr;
    }

    // Configurar el contexto de OpenGL 3.3 Core Profile || Configure OpenGL 3.3 Core Profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Configurar limites del tamaño de ventana || Configure window size limits
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    // Obtener dimensiones del monitor principal || Get main monitor dimensions
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    widthR = mode->width;
    heightR = mode->height;

    // Crear objeto de ventana con GLFW || Create window object using GLFW
    GLFWwindow* window = glfwCreateWindow(widthR, heightR, title.c_str(), NULL, NULL);
    if (!window)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return nullptr;
    }

    // Establecer el contexto actual para el hilo de ejecucion || Set current context for the execution thread
    glfwMakeContextCurrent(window);
    return window;
}

// Cargar y asignar el icono del sistema a la ventana || Load and assign system icon to the window
void LoadWindowIcon(GLFWwindow* window, const std::string& path)
{
    int width, height, channels;
    // Cargar pixeles del archivo utilizando SOIL || Load file pixels using SOIL
    unsigned char* pixels = SOIL_load_image(path.c_str(), &width, &height, &channels, 4);

    if (pixels) {
        GLFWimage images[1];
        images[0].width = width;
        images[0].height = height;
        images[0].pixels = pixels;

        // Establecer la imagen cargada como icono || Set loaded image as icon
        glfwSetWindowIcon(window, 1, images);
        free(pixels);
    }
    else {
        std::cout << "Error cargando icono con SOIL\n";
    }
}

// Inicializar extensiones y estados graficos globales || Initialize extensions and global graphic states
bool InitOpenGLState()
{
    // Habilitar funciones experimentales de GLEW || Enable experimental GLEW functions
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error inicializando GLEW\n";
        return false;
    }

    // Activar pruebas de profundidad, transparencias y descarte de caras || Enable depth testing, blending and face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    return true;
}
