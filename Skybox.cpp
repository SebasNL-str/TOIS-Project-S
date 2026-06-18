#include "Skybox.h"

#include <SOIL2/SOIL2.h>

// Constructor para skybox de tipo cubo || Constructor for cube type skybox
Skybox::Skybox(const std::vector<std::string>& faces) {
    this->type = SkyboxType::Cube;
    cubemapTexture = loadCubemap(faces);
    setupCube();
}

// Constructor para skybox de tipo esferico || Constructor for spherical type skybox
Skybox::Skybox(const std::string& texturePath, SkyboxType type) {
    this->type = type;
    if (type == SkyboxType::Sphere) {
        sphereTexture = loadTexture2D(texturePath);
        setupSphere();
    }
}

// Cargar una textura bidimensional estandar || Load a standard two dimensional texture
GLuint Skybox::loadTexture2D(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int w, h, ch;
    unsigned char* data = SOIL_load_image(path.c_str(), &w, &h, &ch, SOIL_LOAD_RGBA);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Configurar envoltura y filtros de la textura || Configure texture wrapping and filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        free(data);
    }
    return textureID;
}

// Cargar las seis caras del mapa de cubos || Load the six faces of the cube map
GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int w, h, ch;
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data = SOIL_load_image(faces[i].c_str(), &w, &h, &ch, SOIL_LOAD_RGBA);
        if (data) {
            // Asignar los pixeles a la cara correspondiente || Assign pixels to the corresponding face
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            free(data);
        }
    }

    // Configurar envoltura y filtros del mapa de cubos || Configure cube map wrapping and filters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return textureID;
}

// Inicializar buffers del cubo de fondo || Initialize background cube buffers
void Skybox::setupCube() {
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
    };

    // Registrar geometria en buffers de OpenGL || Register geometry in OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

// Renderizar el skybox activo en la escena || Render the active skybox in the scene
void Skybox::Draw(Shader& shader, Camera& camera, float width, float height) {
    // Configurar estados de profundidad para el fondo || Configure depth states for the background
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    shader.Use();

    // Remover traslacion de la matriz de vista || Remove translation from view matrix
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), width / height, 0.1f, 100.0f);

    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);

    // Dibujar segun el tipo de proyeccion || Draw based on projection type
    if (type == SkyboxType::Cube) {
        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0); // 1. Forzar el uso de la unidad de textura 0
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        shader.SetInt("skybox", 0); // 2. Vincular el uniform del fragment shader a la unidad 0

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    else if (type == SkyboxType::Sphere) {
        glBindVertexArray(sphereVAO);
        glBindTexture(GL_TEXTURE_2D, sphereTexture);
        glDrawElements(GL_TRIANGLE_STRIP, sphereIndexCount, GL_UNSIGNED_INT, 0);
    }

    // Restaurar configuracion original de profundidad || Restore original depth configuration
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

// Generar una malla esferica mediante calculos trigonometricos || Generate a spherical mesh using trigonometric calculations
void Skybox::setupSphere(unsigned int XSegments, unsigned int YSegments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Calcular puntos y coordenadas UV de la esfera || Calculate points and UV coordinates of the sphere
    for (unsigned int y = 0; y <= YSegments; ++y) {
        for (unsigned int x = 0; x <= XSegments; ++x) {
            float xSegment = (float)x / (float)XSegments;
            float ySegment = (float)y / (float)YSegments;
            float xPos = std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
            float yPos = std::cos(ySegment * glm::pi<float>());
            float zPos = std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xSegment);
            vertices.push_back(ySegment);
        }
    }

    // Construir la lista de indices para el dibujo indexado || Build the index list for indexed drawing
    for (unsigned int y = 0; y < YSegments; ++y) {
        for (unsigned int x = 0; x <= XSegments; ++x) {
            indices.push_back(y * (XSegments + 1) + x);
            indices.push_back((y + 1) * (XSegments + 1) + x);
        }
    }

    sphereIndexCount = (GLsizei)indices.size();

    // Enviar arreglos a los buffers dinamicos de OpenGL || Send arrays to OpenGL dynamic buffers
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}
