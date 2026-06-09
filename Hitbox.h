#pragma once
// GLM includes
#include <glm.hpp>

// std includes
#include <vector>
#include <cfloat>
#include <algorithm>

// Project includes
#include "Shader.h"

// Caja de todo el modelo || Model's bounding box
struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

// Estructura de la caja de colision de la camara || Camera collider structure
struct CameraCollider {
    glm::vec3 position;
    float radius; // tamaño de la esfera de colisión || Sphere radius size
};

// Sin usar (Debug antiguo) || Unused (Old debug)
struct ConvexHull {
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> faces; // índices de triángulos del hull
};

// Colision por mallas de modelo || Model mesh collision type
struct MeshCollider {
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> faces;

    // Variables de OpenGL existentes || Actual OpenGL variables
    unsigned int VAO = 0, VBO = 0, EBO = 0, indexCount = 0;

    // Caja contenedora (AABB) || Container of max and min
    glm::vec3 minAABB{ FLT_MAX };
    glm::vec3 maxAABB{ -FLT_MAX };

    // Buffers exclusivos para dibujar el cubo de la AABB || Exclusive buffers to draw the cube around the model
    unsigned int aabbVAO = 0;
    unsigned int aabbVBO = 0;
    unsigned int aabbEBO = 0;
};

// Calcula una AABB a partir de una lista de posiciones || Matches up a bounding box with a positions list
BoundingBox CalculateBoundingBox(const std::vector<glm::vec3>& vertices);
bool CheckCollisionCameraBox(const CameraCollider& cam, const BoundingBox& box);
BoundingBox TransformBoundingBox(const BoundingBox& box, const glm::mat4& modelMatrix);

// Colisión entre dos AABB || Collision between two bounding boxes
bool CheckCollision(const BoundingBox& a, const BoundingBox& b);

// Dibujar la hitbox (Bbunding box) en modo wireframe (debug, sin usar) || Draws model wireframe hitbox (bounding box) (debug, unused)
void DrawHitbox(const BoundingBox& box, Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);

// Debug (sin utilizar) || Debug (unused)
void DrawConvexHull(const ConvexHull& hull, Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);

// Dibujar la hitbox conforme a la malla del modelo || Draws the meshes of the model as a hitbox
void DrawMeshCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);

// Verifica si la esfera (collider de la camara) choca con la hitbox del modelo || Self-explanatory
bool CheckCollisionSphereMesh(const glm::vec3& center, float radius,
    const MeshCollider& mesh,
    const glm::mat4& modelMatrix);

// Debug (sin usar) || // Debug (unused)
bool CheckCollisionSphereTriangle(const glm::vec3& center, float radius,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c);

// Verifica puntos en comun de triangulos || Checks common points between triangles
bool PointInTriangle(const glm::vec3& p,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c);

// Crea los limites de colision del modelo con respecto a la malla || Makes the collision bounds of the model according to its mesh
void SetupMeshCollider(MeshCollider& mesh);

void DrawAABBCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color);