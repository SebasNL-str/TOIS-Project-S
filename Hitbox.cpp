#include "Hitbox.h"
#include <cfloat>
#include <algorithm>
#include "Shader.h"
#include <GL/gl.h> // OpenGL puro
#include <gtc/type_ptr.hpp>


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

void DrawMeshCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color)
{
    // Si no se ha inicializado el VAO, salimos de inmediato
    if (mesh.VAO == 0 || mesh.indexCount == 0) return;

    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("color", color);

    // Dibujar utilizando el VAO guardado en memoria
    glBindVertexArray(mesh.VAO);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Modo Wireframe
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restaurar modo sólido

    glBindVertexArray(0);
}


bool PointInTriangle(const glm::vec3& p,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c)
{
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool CheckCollisionSphereTriangle(const glm::vec3& center, float radius,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c)
{
    glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
    float dist = glm::dot(center - a, normal);

    if (fabs(dist) > radius) return false;

    glm::vec3 proj = center - dist * normal;
    return PointInTriangle(proj, a, b, c);
}

bool CheckCollisionSphereMesh(const glm::vec3& center, float radius,
    const MeshCollider& mesh,
    const glm::mat4& modelMatrix)
{
    // 1. Convertir el centro de la esfera (Cámara) al espacio local del objeto
    // Esto nos permite evitar transformar miles de vértices por frame
    glm::mat4 inverseModel = glm::inverse(modelMatrix);
    glm::vec3 localCenter = glm::vec3(inverseModel * glm::vec4(center, 1.0f));

    // 2. FILTRO RÁPIDO: Verificación Esfera vs Caja Contenedora (AABB)
    // Encuentra el punto más cercano de la caja a la esfera
    float closureX = glm::max(mesh.minAABB.x, glm::min(localCenter.x, mesh.maxAABB.x));
    float closureY = glm::max(mesh.minAABB.y, glm::min(localCenter.y, mesh.maxAABB.y));
    float closureZ = glm::max(mesh.minAABB.z, glm::min(localCenter.z, mesh.maxAABB.z));

    // Calcula la distancia al cuadrado entre ese punto y el centro de la esfera
    float distanceSq = ((closureX - localCenter.x) * (closureX - localCenter.x)) +
        ((closureY - localCenter.y) * (closureY - localCenter.y)) +
        ((closureZ - localCenter.z) * (closureZ - localCenter.z));

    // Si la distancia es mayor que el radio al cuadrado, NO HAY COLISIÓN. Omitimos la malla.
    if (distanceSq > (radius * radius)) {
        return false;
    }

    // 3. FASE ESTRECHA: Si la esfera está tocando la caja, revisamos los triángulos
    // Como trabajamos en espacio local, ya NO multiplicamos por 'modelMatrix' aquí dentro
    for (const auto& face : mesh.faces) {
        const glm::vec3& a = mesh.vertices[face.x];
        const glm::vec3& b = mesh.vertices[face.y];
        const glm::vec3& c = mesh.vertices[face.z];

        if (CheckCollisionSphereTriangle(localCenter, radius, a, b, c)) {
            return true; // Colisión real detectada
        }
    }
    return false;
}


void SetupMeshCollider(MeshCollider& mesh)
{
    // Si no hay datos cargados en la CPU, salimos para evitar errores de OpenGL
    if (mesh.vertices.empty() || mesh.faces.empty()) return;

    // 1. Convertir caras (ivec3) a un vector plano de índices
    std::vector<unsigned int> indices;
    indices.reserve(mesh.faces.size() * 3);
    for (const auto& f : mesh.faces) {
        indices.push_back(static_cast<unsigned int>(f.x));
        indices.push_back(static_cast<unsigned int>(f.y));
        indices.push_back(static_cast<unsigned int>(f.z));
    }
    mesh.indexCount = static_cast<unsigned int>(indices.size());

    // 2. Generar buffers en la GPU
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    // 3. Enlazar y rellenar Vertex Array Object
    glBindVertexArray(mesh.VAO);

    // VBO: Subir vértices (Cada elemento es un glm::vec3 puro)
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        mesh.vertices.size() * sizeof(glm::vec3),
        mesh.vertices.data(),
        GL_STATIC_DRAW);

    // EBO: Subir los índices de los triángulos
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // 4. Configurar atributos de vértices (Atributo 0 = Posición)
    // El stride ahora es exactamente sizeof(glm::vec3) porque no hay normales ni UVs aquí
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Desenlazar para proteger el estado de OpenGL
    glBindVertexArray(0);
}


void DrawAABBCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color)
{
    if (mesh.aabbVAO == 0) return;

    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("color", color);

    glBindVertexArray(mesh.aabbVAO);

    // Dibujamos líneas directas para ver la caja contenedora
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}







