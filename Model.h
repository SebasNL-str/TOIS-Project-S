#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include "Mesh.h"
#include "Hitbox.h"

class Model
{
public:
    // Constructores
    Model(const std::string& path);
    Model(const std::string& path, const std::string& fallbackTexture);

    // Dibujar el modelo completo
    void Draw();
    BoundingBox hitbox;
    std::vector<glm::vec3> allVertices;


private:
    // Lista de mallas
    std::vector<Mesh> meshes;

    std::string modelDirectory;

    // Textura de respaldo (fallback)
    GLuint fallbackTexture = 0;

    // =========================
    // Funciones internas
    // =========================
    void loadModel(const std::string& path);

    // Recorrido recursivo de nodos
    void processNode(struct aiNode* node, const struct aiScene* scene, glm::mat4 parentTransform);

    // Procesamiento de cada malla
    void processMesh(struct aiMesh* mesh, const struct aiScene* scene, glm::mat4 transform);

    // Carga de texturas desde archivo
    GLuint loadTexture(const std::string& path);
    std::string resolveTexturePath(const std::string& texturePath) const;
};
