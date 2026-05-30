#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include "Shader.h"
#include "Camera.h"

enum class SkyboxType {
    Cube,
    Sphere
};

class Skybox {
public:
    // Cubemap
    Skybox(const std::vector<std::string>& faces);

    // Esfera
    Skybox(const std::string& texturePath, SkyboxType type);

    void Draw(Shader& shader, Camera& camera, float width, float height);

    SkyboxType type;

private:
    // --- Cubemap ---
    GLuint cubemapTexture;
    GLuint VAO, VBO;

    // --- Esfera ---
    GLuint sphereTexture;
    GLuint sphereVAO, sphereVBO, sphereEBO;
    GLsizei sphereIndexCount = 0;

    // Métodos
    GLuint loadCubemap(const std::vector<std::string>& faces);
    GLuint loadTexture2D(const std::string& path);
    void setupCube();
    void setupSphere(unsigned int XSegments = 64, unsigned int YSegments = 64);
};
