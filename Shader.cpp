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
    // =========================
    // LEER ARCHIVOS
    // =========================

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

    // =========================
    // VERTEX SHADER
    // =========================

    GLuint vertex;

    vertex =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertex,
        1,
        &vShaderCode,
        NULL
    );

    glCompileShader(vertex);

    // =========================
    // FRAGMENT SHADER
    // =========================

    GLuint fragment;

    fragment =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragment,
        1,
        &fShaderCode,
        NULL
    );

    glCompileShader(fragment);

    // =========================
    // PROGRAM
    // =========================

    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::SetFloat(const std::string& name, float value)
{
    glUniform1f(
        glGetUniformLocation(ID, name.c_str()),
        value
    );
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value)
{
    glUniform3f(
        glGetUniformLocation(ID, name.c_str()),
        value.x,
        value.y,
        value.z
    );
}

void Shader::Use()
{
    glUseProgram(ID);
}

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
