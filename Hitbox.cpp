#include "Hitbox.h"
#include "Shader.h"

#include <cfloat>
#include <algorithm>

#include <GL/gl.h>
#include <gtc/type_ptr.hpp>

// Comprobar la interseccion entre la camara esferica y una caja de colision || Check intersection between spherical camera and a bounding box
bool CheckCollisionCameraBox(const CameraCollider& cam, const BoundingBox& box) {
    // Delimitar la posicion de la camara al rango interno de la caja || Clamp camera position to the inner range of the box
    float x = std::max(box.min.x, std::min(cam.position.x, box.max.x));
    float y = std::max(box.min.y, std::min(cam.position.y, box.max.y));
    float z = std::max(box.min.z, std::min(cam.position.z, box.max.z));

    // Calcular la distancia al cuadrado hacia el punto mas cercano || Calculate squared distance to the nearest point
    float distSq = (x - cam.position.x) * (x - cam.position.x) +
        (y - cam.position.y) * (y - cam.position.y) +
        (z - cam.position.z) * (z - cam.position.z);

    return distSq < (cam.radius * cam.radius);
}

// Calcular los limites maximos y minimos de un arreglo de vertices || Calculate maximum and minimum bounds of a vertex array
BoundingBox CalculateBoundingBox(const std::vector<glm::vec3>& vertices) {
    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);

    // Recorrer los vertices para encontrar los extremos extruidos || Loop through vertices to find extruded extremes
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

// Transformar una caja de colision local al espacio tridimensional del mundo || Transform a local bounding box to three dimensional world space
BoundingBox TransformBoundingBox(const BoundingBox& box, const glm::mat4& modelMatrix) {
    // Definir las coordenadas locales de las ocho esquinas del cubo || Define local coordinates for the eight corners of the cube
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

    // Multiplicar cada esquina por la matriz del modelo correspondientemente || Multiply each corner by the model matrix correspondingly
    for (int i = 0; i < 8; i++) {
        glm::vec4 transformed = modelMatrix * glm::vec4(corners[i], 1.0f);
        minBounds = glm::min(minBounds, glm::vec3(transformed));
        maxBounds = glm::max(maxBounds, glm::vec3(transformed));
    }

    return { minBounds, maxBounds };
}

// Comprobar la interseccion entre dos cajas de colision estructuradas || Check intersection between two structured bounding boxes
bool CheckCollision(const BoundingBox& a, const BoundingBox& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// Renderizar las aristas lineales de una caja de colision en el mundo || Render linear edges of a bounding box in the world
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

    // Registrar buffers graficos locales temporales || Register temporary local graphic buffers
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

    // Activar shader y transferir matrices uniformes calculadas || Activate shader and transfer calculated uniform matrices
    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("color", color);

    // Configurar rasterizado en modo de lineas para las aristas || Configure rasterization in line mode for edges
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Liberar memoria local de los buffers || Release buffers local memory
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// Renderizar el contorno alambrado de la malla de colision || Render the wireframe outline of the mesh collider
void DrawMeshCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color)
{
    // Salir si los identificadores de memoria no son validos || Exit if memory identifiers are not valid
    if (mesh.VAO == 0 || mesh.indexCount == 0) return;

    // Enviar estados y matrices de proyeccion al shader || Send states and projection matrices to the shader
    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("color", color);

    // Enlazar buffer de vertices estructurado || Bind structured vertex buffer
    glBindVertexArray(mesh.VAO);

    // Alternar rasterizado en modo wireframe y dibujar || Toggle wireframe rasterization mode and draw
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);
}

// Determinar si un punto proyectado se encuentra dentro de un triangulo || Determine if a projected point lies inside a triangle
bool PointInTriangle(const glm::vec3& p,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c)
{
    // Calcular vectores de coordenadas baricentricas || Calculate barycentric coordinate vectors
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    // Resolver la ecuacion de proyeccion plana || Solve the planar projection equation
    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}

// Comprobar la colision entre una esfera y un plano triangular || Check collision between a sphere and a triangular plane
bool CheckCollisionSphereTriangle(const glm::vec3& center, float radius,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c)
{
    // Obtener vector normal y distancia coplanar del punto || Get normal vector and coplanar distance of the point
    glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
    float dist = glm::dot(center - a, normal);

    if (fabs(dist) > radius) return false;

    // Evaluar proyeccion ortogonal dentro del triangulo || Evaluate orthogonal projection inside the triangle
    glm::vec3 proj = center - dist * normal;
    return PointInTriangle(proj, a, b, c);
}

// Comprobar la colision entre una esfera y la malla poligonal || Check collision between a sphere and the polygonal mesh
bool CheckCollisionSphereMesh(const glm::vec3& center, float radius,
    const MeshCollider& mesh,
    const glm::mat4& modelMatrix)
{
    // Transformar el centro de la esfera al espacio local del objeto || Transform the sphere center to object local space
    glm::mat4 inverseModel = glm::inverse(modelMatrix);
    glm::vec3 localCenter = glm::vec3(inverseModel * glm::vec4(center, 1.0f));

    // Ejecutar filtro rapido mediante caja contenedora local || Execute fast filter using local bounding box
    float closureX = glm::max(mesh.minAABB.x, glm::min(localCenter.x, mesh.maxAABB.x));
    float closureY = glm::max(mesh.minAABB.y, glm::min(localCenter.y, mesh.maxAABB.y));
    float closureZ = glm::max(mesh.minAABB.z, glm::min(localCenter.z, mesh.maxAABB.z));

    // Calcular distancia escalar al cuadrado || Calculate squared scalar distance
    float distanceSq = ((closureX - localCenter.x) * (closureX - localCenter.x)) +
        ((closureY - localCenter.y) * (closureY - localCenter.y)) +
        ((closureZ - localCenter.z) * (closureZ - localCenter.z));

    // Omitir calculos costosos si no interseca la caja contenedora || Skip costly calculations if bounding box does not intersect
    if (distanceSq > (radius * radius)) {
        return false;
    }

    // Recorrer de forma secuencial cada una de las caras poligonales || Sequentially loop through each of the polygonal faces
    for (const auto& face : mesh.faces) {
        const glm::vec3& a = mesh.vertices[face.x];
        const glm::vec3& b = mesh.vertices[face.y];
        const glm::vec3& c = mesh.vertices[face.z];

        // Validar colision real contra el plano del triangulo || Validate real collision against the triangle plane
        if (CheckCollisionSphereTriangle(localCenter, radius, a, b, c)) {
            return true;
        }
    }
    return false;
}

// Configurar y cargar los buffers de la malla de colision en la GPU || Configure and load the mesh collider buffers into the GPU
void SetupMeshCollider(MeshCollider& mesh)
{
    // Salir si los vectores en memoria de CPU estan vacios || Exit if CPU memory vectors are empty
    if (mesh.vertices.empty() || mesh.faces.empty()) return;

    // Convertir componentes vectoriales a un arreglo plano de indices || Convert vector components to a flat index array
    std::vector<unsigned int> indices;
    indices.reserve(mesh.faces.size() * 3);
    for (const auto& f : mesh.faces) {
        indices.push_back(static_cast<unsigned int>(f.x));
        indices.push_back(static_cast<unsigned int>(f.y));
        indices.push_back(static_cast<unsigned int>(f.z));
    }
    mesh.indexCount = static_cast<unsigned int>(indices.size());

    // Generar identificadores de buffers de OpenGL || Generate OpenGL buffer identifiers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    // Cargar el arreglo de coordenadas de vertices || Load the vertex coordinates array
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        mesh.vertices.size() * sizeof(glm::vec3),
        mesh.vertices.data(),
        GL_STATIC_DRAW);

    // Cargar el arreglo de indices estructurados || Load the structured indices array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // Configurar atributo de posiciones tridimensionales || Configure three dimensional positions attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

// Renderizar las aristas de la caja contenedora de la malla || Render the edges of the mesh bounding box
void DrawAABBCollider(const MeshCollider& mesh,
    Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color)
{
    // Validar si el buffer del objeto fue generado previamente || Validate if the object buffer was previously generated
    if (mesh.aabbVAO == 0) return;

    // Activar shader y enviar variables uniformes transformadas || Activate shader and send transformed uniform variables
    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("color", color);

    // Enlazar buffer y dibujar lineas primitivas || Bind buffer and draw primitive lines
    glBindVertexArray(mesh.aabbVAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}








