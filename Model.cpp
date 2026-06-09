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


// Convertir la matriz nativa de Assimp a una matriz compatible de GLM || Convert Assimp native matrix to a GLM compatible matrix
glm::mat4 aiToGlm(const aiMatrix4x4& m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

// Cargar un mapa de bits y generar un objeto de textura OpenGL || Load a bitmap image and generate an OpenGL texture object
GLuint Model::loadTexture(const std::string& filename)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Establecer parametros por defecto del mapeo lineal || Set default linear mapping parameters
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

        // Evaluar canal alfa para identificar transparencias || Evaluate alpha channel to identify transparencies
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

        // Cambiar envoltura si requiere mascaras transparentes || Change wrapping if it requires transparent masks
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

        // Cargar textura blanca solida de respaldo en fallo || Load solid white fallback texture on failure
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, white);
    }

    return texture;
}

// Resolver y verificar la ruta de almacenamiento de la textura || Resolve and verify the texture storage path
std::string Model::resolveTexturePath(const std::string& texturePath) const
{
    if (texturePath.empty())
    {
        return texturePath;
    }

    // Validar si la cadena contiene una ruta absoluta || Validate if string contains an absolute path
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

    // Construir vector con las posibles rutas relativas || Build vector with possible relative paths
    std::vector<std::string> candidates;
    if (!modelDirectory.empty())
    {
        candidates.push_back(modelDirectory + "/" + normalizedPath);
    }
    candidates.push_back("Resources/Models/" + normalizedPath);
    candidates.push_back(normalizedPath);

    // Iterar para encontrar el archivo en el sistema de archivos || Iterate to locate the file in the file system
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

// Constructor basico del modelo tridimensional || Basic constructor for the three dimensional model
Model::Model(const std::string& path)
{
    fallbackTexture = 0;
    loadModel(path);
}

// Constructor del modelo con textura alternativa definida || Model constructor with defined alternative texture
Model::Model(const std::string& path, const std::string& fallbackTexturePath)
{
    fallbackTexture = loadTexture(fallbackTexturePath);
    loadModel(path);
}

// Renderizar secuencialmente todas las mallas del objeto || Render sequentially all meshes of the object
void Model::Draw()
{
    for (auto& mesh : meshes)
        mesh.Draw();
}


// Cargar y procesar un archivo de modelo tridimensional con Assimp || Load and process a three dimensional model file with Assimp
void Model::loadModel(const std::string& path)
{
    std::string normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

    // Extraer el directorio base del archivo para resolver texturas || Extract file base directory to resolve textures
    std::size_t lastSlash = normalizedPath.find_last_of('/');
    modelDirectory = lastSlash == std::string::npos ? "" : normalizedPath.substr(0, lastSlash);

    // Configurar e invocar el importador con banderas de optimizacion || Configure and invoke the importer with optimization flags
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

    // Validar la integridad de los datos cargados por el importador || Validate data integrity loaded by the importer
    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Iniciar el recorrido recursivo desde el nodo raiz || Start recursive traversal from the root node
    processNode(scene->mRootNode, scene, glm::mat4(1.0f));

    // Calcular la caja contenedora global del objeto || Calculate global object bounding box
    hitbox = CalculateBoundingBox(allVertices);
}

// Recorrer de forma recursiva el arbol de nodos y aplicar transformaciones || Recursively traverse the node tree and apply transformations
void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform)
{
    // Concatenar transformaciones relativas del nodo actual || Concatenate relative transformations of the current node
    glm::mat4 nodeTransform = parentTransform * aiToGlm(node->mTransformation);

    // Procesar cada una de las mallas vinculadas al nodo || Process each of the meshes linked to the node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, nodeTransform);
    }

    // Recorrer los nodos hijos de la jerarquia estructurada || Traverse child nodes of the structured hierarchy
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, nodeTransform);
    }
}

// Procesar los vertices, indices y materiales de una malla individual || Process individual mesh vertices, indices and materials
void Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Almacenar el desfase de vertices previo para el calculo de indices || Store the previous vertex offset for index calculation
    unsigned int vertexOffset = collider.vertices.size();

    // Recorrer los vertices y transformar posiciones y vectores normales || Loop through vertices and transform positions and normal vectors
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v{};
        glm::vec4 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        pos = transform * pos;

        v.x = pos.x;
        v.y = pos.y;
        v.z = pos.z;

        // Validar y transformar vectores de normales || Validate and transform normal vectors
        if (mesh->HasNormals())
        {
            glm::vec4 norm(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
            norm = transform * norm;
            v.nx = norm.x;
            v.ny = norm.y;
            v.nz = norm.z;
        }

        // Asignar coordenadas de mapeo UV || Assign UV mapping coordinates
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

        // Alimentar estructuras de colision con los puntos del mundo || Feed collision structures with world space points
        glm::vec3 finalPos(v.x, v.y, v.z);
        collider.vertices.push_back(finalPos);
        allVertices.push_back(finalPos);

        // Expandir las dimensiones de la caja contenedora local || Expand local bounding box dimensions
        collider.minAABB = glm::min(collider.minAABB, finalPos);
        collider.maxAABB = glm::max(collider.maxAABB, finalPos);
    }

    // Corregir indices de las caras triangulares del colisionador || Correct indices for the collider triangular faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices == 3) {
            collider.faces.push_back(glm::ivec3(
                face.mIndices[0] + vertexOffset,
                face.mIndices[1] + vertexOffset,
                face.mIndices[2] + vertexOffset
            ));
        }
    }

    // Almacenar los indices locales de renderizado visual || Store local indices for visual rendering
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString pathTex;

    GLuint diffuseTex = 0;

    // Asignar la textura por defecto de respaldo || Assign fallback texture by default
    if (fallbackTexture != 0) {
        diffuseTex = fallbackTexture;
    }

    // Cargar mapa de textura difusa del material || Load material diffuse texture map
    if (diffuseTex == 0 && material &&
        material->GetTexture(aiTextureType_DIFFUSE, 0, &pathTex) == AI_SUCCESS) {
        diffuseTex = loadTexture(resolveTexturePath(pathTex.C_Str()));
    }

    // Generar textura de color solido si no tiene imagen || Generate solid color texture if it has no image
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

    // Generar textura blanca de respaldo absoluto || Generate absolute fallback white texture
    if (diffuseTex == 0) {
        unsigned char white[3] = { 255, 255, 255 };
        glGenTextures(1, &diffuseTex);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
            GL_RGB, GL_UNSIGNED_BYTE, white);
    }

    // Cargar mapa de rugosidad del material || Load material roughness map
    GLuint roughnessTex = 0;
    if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &pathTex) == AI_SUCCESS) {
        roughnessTex = loadTexture(resolveTexturePath(pathTex.C_Str()));
    }

    // Cargar mapa de metalicidad del material || Load material metallic map
    GLuint metallicTex = 0;
    if (material->GetTexture(aiTextureType_METALNESS, 0, &pathTex) == AI_SUCCESS) {
        metallicTex = loadTexture(resolveTexturePath(pathTex.C_Str()));
    }

    // Registrar e instanciar objeto visual Mesh || Register and instantiate visual Mesh object
    meshes.emplace_back(vertices, indices, diffuseTex, roughnessTex, metallicTex);
}


