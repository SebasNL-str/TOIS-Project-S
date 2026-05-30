#include "Hitbox.h"
#include <cfloat>
#include <algorithm>
#include "Shader.h"
#include <GL/gl.h> // OpenGL puro

bool CheckCollisionCameraBox(const CameraCollider& cam, const BoundingBox& box) {
    // Clampear la posición de la cámara al rango de la caja
    float x = std::max(box.min.x, std::min(cam.position.x, box.max.x));
    float y = std::max(box.min.y, std::min(cam.position.y, box.max.y));
    float z = std::max(box.min.z, std::min(cam.position.z, box.max.z));

    // Distancia desde la cámara al punto más cercano de la caja
    float distSq = (x - cam.position.x) * (x - cam.position.x) +
        (y - cam.position.y) * (y - cam.position.y) +
        (z - cam.position.z) * (z - cam.position.z);

    return distSq < (cam.radius * cam.radius);
}


BoundingBox CalculateBoundingBox(const std::vector<glm::vec3>& vertices) {
    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);

    for (const auto& v : vertices) {
        minBounds.x = std::min(minBounds.x, v.x);
        minBounds.y = std::min(minBounds.y, v.y);
        minBounds.z = std::min(minBounds.z, v.z);

        maxBounds.x = std::max(maxBounds.x, v.x);
        maxBounds.y = std::max(maxBounds.y, v.y);
        maxBounds.z = std::max(maxBounds.z, v.z);
    }

    return { minBounds, maxBounds };
}

BoundingBox TransformBoundingBox(const BoundingBox& box, const glm::mat4& modelMatrix) {
    glm::vec3 corners[8] = {
        {box.min.x, box.min.y, box.min.z},
        {box.max.x, box.min.y, box.min.z},
        {box.max.x, box.max.y, box.min.z},
        {box.min.x, box.max.y, box.min.z},
        {box.min.x, box.min.y, box.max.z},
        {box.max.x, box.min.y, box.max.z},
        {box.max.x, box.max.y, box.max.z},
        {box.min.x, box.max.y, box.max.z}
    };

    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);

    for (int i = 0; i < 8; i++) {
        glm::vec4 transformed = modelMatrix * glm::vec4(corners[i], 1.0f);
        minBounds = glm::min(minBounds, glm::vec3(transformed));
        maxBounds = glm::max(maxBounds, glm::vec3(transformed));
    }

    return { minBounds, maxBounds };
}


bool CheckCollision(const BoundingBox& a, const BoundingBox& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

void DrawHitbox(const BoundingBox& box, Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color)
{
    float vertices[] = {
        box.min.x, box.min.y, box.min.z,
        box.max.x, box.min.y, box.min.z,
        box.max.x, box.max.y, box.min.z,
        box.min.x, box.max.y, box.min.z,
        box.min.x, box.min.y, box.max.z,
        box.max.x, box.min.y, box.max.z,
        box.max.x, box.max.y, box.max.z,
        box.min.x, box.max.y, box.max.z,
    };

    unsigned int indices[] = {
        0,1, 1,2, 2,3, 3,0,
        4,5, 5,6, 6,7, 7,4,
        0,4, 1,5, 2,6, 3,7
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("color", color);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
