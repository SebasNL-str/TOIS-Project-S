#pragma once

// std includes
#include <vector>
#include <string>

// glm include
#include <glm.hpp>

// Project includes
#include "Mesh.h"
#include "Hitbox.h"

class Model
{
public:
    // Constructores || Constructors
    Model(const std::string& path);
    Model(const std::string& path, const std::string& fallbackTexture);

    // Dibujar el modelo completo || Draws the entire model
    void Draw();

    // Hitboxes
    BoundingBox hitbox;              // AABB
    ConvexHull hull;                 // Convex Hull
    std::vector<glm::vec3> allVertices; // Todos los vértices del modelo || All models vertices
    MeshCollider collider;

private:
    // Lista de mallas || Mesh list
    std::vector<Mesh> meshes;

    std::string modelDirectory;

    // Textura de respaldo (fallback) || Default texture
    GLuint fallbackTexture = 0;

    // =========================
    // Funciones internas || Internal functions
    // =========================
    void loadModel(const std::string& path);

    // Recorrido recursivo de nodos || Recursive haul throughout the nodes
    void processNode(struct aiNode* node, const struct aiScene* scene, glm::mat4 parentTransform);

    // Procesamiento de cada malla || Mesh processing
    void processMesh(struct aiMesh* mesh, const struct aiScene* scene, glm::mat4 transform);

    // Carga de texturas desde archivo || Texture loading from files
    GLuint loadTexture(const std::string& path);
    std::string resolveTexturePath(const std::string& texturePath) const;
};
