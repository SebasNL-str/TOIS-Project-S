#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <gtc/type_ptr.hpp>

Shader::Shader(
    const char* vertexPath,
    const char* fragmentPath
)
{
    // Cargar codigo fuente desde los archivos de disco || Load source code from disk files
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);

    std::stringstream vStream;
    std::stringstream fStream;

    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    std::string vertexCode =
        vStream.str();

    std::string fragmentCode =
        fStream.str();

    const char* vShaderCode =
        vertexCode.c_str();

    const char* fShaderCode =
        fragmentCode.c_str();

    std::cout << "Vertex path: " << vertexPath << std::endl;
    std::cout << "Fragment path: " << fragmentPath << std::endl;

    // Verificar si los archivos se abrieron correctamente || Check if files were opened correctly
    if (!vFile.is_open())
    {
        std::cout << "ERROR vertex shader no abre: "
            << vertexPath << std::endl;
    }

    if (!fFile.is_open())
    {
        std::cout << "ERROR fragment shader no abre: "
            << fragmentPath << std::endl;
    }

    GLuint vertex;

    // Crear y compilar el Vertex Shader de OpenGL || Create and compile the OpenGL Vertex Shader
    vertex =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertex,
        1,
        &vShaderCode,
        NULL
    );

    glCompileShader(vertex);

    GLuint fragment;

    // Crear y compilar el Fragment Shader de OpenGL || Create and compile the OpenGL Fragment Shader
    fragment =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragment,
        1,
        &fShaderCode,
        NULL
    );

    glCompileShader(fragment);

    // Crear el programa de shader y enlazar componentes || Create shader program and link components
    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glLinkProgram(ID);

    // Liberar memoria de los shaders individuales || Free individual shaders memory
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Enviar un valor flotante al shader uniforme || Send a float value to the uniform shader
void Shader::SetFloat(const std::string& name, float value)
{
    glUniform1f(
        glGetUniformLocation(ID, name.c_str()),
        value
    );
}

// Enviar un vector de 3 componentes al shader uniforme || Send a 3-component vector to the uniform shader
void Shader::SetVec3(const std::string& name, const glm::vec3& value)
{
    glUniform3f(
        glGetUniformLocation(ID, name.c_str()),
        value.x,
        value.y,
        value.z
    );
}

// Activar el programa de shader actual || Activate the current shader program
void Shader::Use()
{
    glUseProgram(ID);
}

// Enviar una matriz de 4x4 al shader uniforme || Send a 4x4 matrix to the uniform shader
void Shader::SetMat4(
    const std::string& name,
    glm::mat4 matrix
)
{
    glUniformMatrix4fv(
        glGetUniformLocation(
            ID,
            name.c_str()
        ),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

// Enviar un valor entero al shader uniforme || Send an integer value to the uniform shader
void Shader::SetInt(
    const std::string& name,
    int value
)
{
    glUniform1i(
        glGetUniformLocation(
            ID,
            name.c_str()
        ),
        value
    );
}

// Enviar un valor booleano al shader uniforme || Send a boolean value to the uniform shader
void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

