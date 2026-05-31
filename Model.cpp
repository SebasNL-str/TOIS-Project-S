#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL2/SOIL2.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


// Convierte aiMatrix4x4 a glm::mat4
glm::mat4 aiToGlm(const aiMatrix4x4& m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

// =========================
// HELPERS
// =========================
GLuint Model::loadTexture(const std::string& filename)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

    if (image)
    {
        bool hasTransparency = false;
        int pixelCount = width * height;
        for (int i = 0; i < pixelCount; ++i)
        {
            if (image[i * 4 + 3] < 250)
            {
                hasTransparency = true;
                break;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, image);

        if (hasTransparency)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        free(image);
    }
    else
    {
        std::cout << "ERROR cargando textura: " << filename << std::endl;
        unsigned char white[4] = { 255, 255, 255, 255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, white);
    }

    return texture;
}

std::string Model::resolveTexturePath(const std::string& texturePath) const
{
    if (texturePath.empty())
    {
        return texturePath;
    }

    if (texturePath.size() > 1 && texturePath[1] == ':')
    {
        return texturePath;
    }

    if (texturePath[0] == '/' || texturePath[0] == '\\')
    {
        return texturePath;
    }

    std::string normalizedPath = texturePath;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

    std::vector<std::string> candidates;
    if (!modelDirectory.empty())
    {
        candidates.push_back(modelDirectory + "/" + normalizedPath);
    }
    candidates.push_back("Resources/Models/" + normalizedPath);
    candidates.push_back(normalizedPath);

    for (const std::string& candidate : candidates)
    {
        std::ifstream file(candidate.c_str(), std::ios::binary);
        if (file.good())
        {
            return candidate;
        }
    }

    return candidates.front();
}

// =========================
// CONSTRUCTORES
// =========================
Model::Model(const std::string& path)
{
    fallbackTexture = 0; // sin fallback explÃ­cito
    loadModel(path);
}

Model::Model(const std::string& path, const std::string& fallbackTexturePath)
{
    fallbackTexture = loadTexture(fallbackTexturePath);
    loadModel(path);
}

// =========================
// DRAW
// =========================
void Model::Draw()
{
    for (auto& mesh : meshes)
        mesh.Draw();
}

// =========================
// LOAD MODEL
// =========================
void Model::loadModel(const std::string& path)
{
    std::string normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

    std::size_t lastSlash = normalizedPath.find_last_of('/');
    modelDirectory = lastSlash == std::string::npos ? "" : normalizedPath.substr(0, lastSlash);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_OptimizeMeshes |
        aiProcess_ImproveCacheLocality
    );

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene, glm::mat4(1.0f));

    hitbox = CalculateBoundingBox(allVertices);
}

// =========================
// PROCESS NODE
// =========================
void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform)
{
    glm::mat4 nodeTransform = parentTransform * aiToGlm(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, nodeTransform);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, nodeTransform);
    }
}

// =========================
// PROCESS MESH
// =========================
void Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 1. Guardamos cuántos vértices globales ya existían ANTES de procesar esta malla.
    // Esto es crucial para que los índices apunten al lugar correcto.
    unsigned int vertexOffset = collider.vertices.size();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v{};
        glm::vec4 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        pos = transform * pos;

        v.x = pos.x;
        v.y = pos.y;
        v.z = pos.z;

        if (mesh->HasNormals())
        {
            glm::vec4 norm(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
            norm = transform * norm;
            v.nx = norm.x;
            v.ny = norm.y;
            v.nz = norm.z;
        }

        if (mesh->mTextureCoords[0])
        {
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            v.u = 0.0f;
            v.v = 0.0f;
        }

        vertices.push_back(v);

        // Llenamos el contenedor del collider y el de quickhull con la misma posición
        glm::vec3 finalPos(v.x, v.y, v.z);
        collider.vertices.push_back(finalPos);
        allVertices.push_back(finalPos);

        // NUEVO: Calcular los extremos de la caja contenedora
        collider.minAABB = glm::min(collider.minAABB, finalPos);
        collider.maxAABB = glm::max(collider.maxAABB, finalPos);
    }

    // 2. Corregimos las caras del collider sumando el 'vertexOffset'
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices == 3) { // Triángulo válido
            collider.faces.push_back(glm::ivec3(
                face.mIndices[0] + vertexOffset,
                face.mIndices[1] + vertexOffset,
                face.mIndices[2] + vertexOffset
            ));
        }
    }

    // Llenado de índices locales de la malla visual (mantienen su flujo local)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString pathTex;

    // =========================================================================
    // DIFUSA (fallback explícito > material > color difuso > blanco)
    // =========================================================================
    GLuint diffuseTex = 0;

    if (fallbackTexture != 0) {
        diffuseTex = fallbackTexture;
    }

    if (diffuseTex == 0 && material &&
        material->GetTexture(aiTextureType_DIFFUSE, 0, &pathTex) == AI_SUCCESS) {
        diffuseTex = loadTexture(resolveTexturePath(pathTex.C_Str()));
    }

    if (diffuseTex == 0) {
        aiColor3D color(0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            unsigned char pixel[3] = {
                (unsigned char)(color.r * 255),
                (unsigned char)(color.g * 255),
                (unsigned char)(color.b * 255)
            };
            glGenTextures(1, &diffuseTex);
            glBindTexture(GL_TEXTURE_2D, diffuseTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
                GL_RGB, GL_UNSIGNED_BYTE, pixel);
        }
    }

    if (diffuseTex == 0) {
        unsigned char white[3] = { 255, 255, 255 };
        glGenTextures(1, &diffuseTex);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
            GL_RGB, GL_UNSIGNED_BYTE, white);
    }

    // =========================================================================
    // ROUGHNESS
    // =========================================================================
    GLuint roughnessTex = 0;
    if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &pathTex) == AI_SUCCESS) {
        roughnessTex = loadTexture(resolveTexturePath(pathTex.C_Str()));
    }

    // =========================================================================
    // METALLIC
    // =========================================================================
    GLuint metallicTex = 0;
    if (material->GetTexture(aiTextureType_METALNESS, 0, &pathTex) == AI_SUCCESS) {
        metallicTex = loadTexture(resolveTexturePath(pathTex.C_Str()));
    }

    // =========================================================================
    // CREAR MESH VISUAL
    // =========================================================================
    meshes.emplace_back(vertices, indices, diffuseTex, roughnessTex, metallicTex);
}

