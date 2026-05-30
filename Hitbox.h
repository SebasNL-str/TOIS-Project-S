#pragma once
#include <glm.hpp>
#include <vector>
#include <cfloat>
#include <algorithm>
#include "Shader.h"

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

struct CameraCollider {
    glm::vec3 position;
    float radius; // tamaño de la esfera de colisión
};


// Calcula una AABB a partir de una lista de posiciones
BoundingBox CalculateBoundingBox(const std::vector<glm::vec3>& vertices);
bool CheckCollisionCameraBox(const CameraCollider& cam, const BoundingBox& box);
BoundingBox TransformBoundingBox(const BoundingBox& box, const glm::mat4& modelMatrix);

// Colisión entre dos AABB
bool CheckCollision(const BoundingBox& a, const BoundingBox& b);

// Dibujar la hitbox en modo wireframe
void DrawHitbox(const BoundingBox& box, Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);
