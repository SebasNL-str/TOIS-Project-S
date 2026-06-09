#pragma once

// std include
#include <string>

// GL include
#define GLEW_STATIC
#include <GL/glew.h>

// GLM include
#include <glm.hpp>

// Clase para la gestion de shaders de OpenGL || Class for OpenGL shader management
class Shader
{
public:

    // Identificador del programa de shader || Shader program ID
    GLuint ID;

    // Constructor que carga y compila los shaders || Constructor that loads and compiles shaders
    Shader(
        const char* vertexPath,
        const char* fragmentPath
    );

    // Activar el programa de shader para su uso || Activate the shader program for use
    void Use();

    // Enviar una matriz de 4x4 al shader || Send a 4x4 matrix to the shader
    void SetMat4(
        const std::string& name,
        glm::mat4 matrix
    );

    // Enviar un valor entero al shader || Send an integer value to the shader
    void SetInt(
        const std::string& name,
        int value
    );

    // Enviar un valor flotante al shader || Send a float value to the shader
    void SetFloat(const std::string& name, float value);

    // Enviar un vector de 3 componentes al shader || Send a 3-component vector to the shader
    void SetVec3(const std::string& name, const glm::vec3& value);

    // Enviar un valor booleano al shader || Send a boolean value to the shader
    void SetBool(const std::string& name, bool value) const;
};

