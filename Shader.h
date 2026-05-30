#pragma once

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm.hpp>



class Shader
{
public:

    GLuint ID;

    Shader(
        const char* vertexPath,
        const char* fragmentPath
    );

    void Use();

    void SetMat4(
        const std::string& name,
        glm::mat4 matrix
    );

    void SetInt(
        const std::string& name,
        int value
    );

    void SetFloat(const std::string& name, float value);
    void SetVec3(const std::string& name, const glm::vec3& value);
};
