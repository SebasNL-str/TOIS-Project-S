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

struct ConvexHull {
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> faces; // índices de triángulos del hull
};

struct MeshCollider {
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> faces;

    // Variables de OpenGL existentes...
    unsigned int VAO = 0, VBO = 0, EBO = 0, indexCount = 0;

    // NUEVO: Caja contenedora (AABB)
    glm::vec3 minAABB{ FLT_MAX };
    glm::vec3 maxAABB{ -FLT_MAX };

    // NUEVO: Buffers exclusivos para dibujar el cubo de la AABB
    unsigned int aabbVAO = 0;
    unsigned int aabbVBO = 0;
    unsigned int aabbEBO = 0;
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

void DrawConvexHull(const ConvexHull& hull, Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);

void DrawMeshCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);

bool CheckCollisionSphereMesh(const glm::vec3& center, float radius,
    const MeshCollider& mesh,
    const glm::mat4& modelMatrix);

bool CheckCollisionSphereTriangle(const glm::vec3& center, float radius,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c);

bool PointInTriangle(const glm::vec3& p,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c);

void SetupMeshCollider(MeshCollider& mesh);

void DrawAABBCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);