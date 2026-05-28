#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Asegúrate de incluir stb_image para la función de cargarCubemap
#include "stb/stb_image.h" 

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Instancias globales
Camera camera(glm::vec3(0.0f, 5.0f, 20.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variables globales para el control del Cubemap (Soluciona la compilación en processInput)
int cubemapActual = 0;
bool teclaCPresionada = false;

// Vértices de un cubo unitario centrado para el Skybox
float skyboxVertices[] = {
    // Posiciones          
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

// Callbacks del Mouse (Fuera del main)
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    static float lastX = 400.0f, lastY = 300.0f;
    static bool firstMouse = true;
    if (firstMouse) { lastX = xposIn; lastY = yposIn; firstMouse = false; }

    float xoffset = xposIn - lastX;
    float yoffset = lastY - yposIn; // Al revés ya que las coordenadas Y van de abajo hacia arriba

    lastX = xposIn; lastY = yposIn;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    // Cerrar la ventana con ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Movimiento de la cámara
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    // Subir con ESPACIO
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);

    // Bajar con CONTROL IZQUIERDO
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    // Atajo para cambiar de Cubemap con la tecla C
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!teclaCPresionada) {
            cubemapActual = (cubemapActual + 1) % 3; // Cicla entre los 3 cubemaps disponibles
            teclaCPresionada = true; // Bloquea hasta que suelte la tecla
            std::cout << "Cambiado a Cubemap ID: " << cubemapActual << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
        teclaCPresionada = false; // Desbloquea la ejecución para el siguiente toque
    }
}

unsigned int cargarCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        // IMPORTANTE: Para los cubemaps NO se deben voltear las imágenes verticalmente.
        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1)      format = GL_RED;
            else if (nrChannels == 4) format = GL_RGBA;

            // OpenGL mapea secuencialmente: POSITIVE_X (0), NEGATIVE_X (1), etc.
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Error: No se pudo cargar la cara del Cubemap en: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Configuración de filtrado y envoltura para evitar costuras visibles en los bordes del cubo
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

int main() {
    // 1. Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configuración de la ventana (OpenGL 3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Assimp + OpenGL", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Capturar y ocultar el cursor para navegación fluida de cámara
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // 2. Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error al inicializar GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // 3. Inicialización de Shaders y Modelos de Assimp
    Shader nuestroShader("default.vert", "default.frag");
    Shader shaderSkybox("skybox.vert", "skybox.frag"); // <--- NUEVO SHADER EXCLUSIVO SKYBOX

    Model model1("Recursos/Modelos/cemen/Cementerio.gltf");


    // --- CONFIGURACIÓN DE GEOMETRÍA VAO/VBO PARA EL SKYBOX ---
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // --- DEFINICIÓN DE RUTAS PARA LOS 3 CUBEMAPS ---
    std::vector<std::string> carasDia = {
        "Recursos/Skybox/Dia/cara_1.png", "Recursos/Skybox/Dia/cara_2.png",
        "Recursos/Skybox/Dia/cara_3.png",   "Recursos/Skybox/Dia/cara_4.png",
        "Recursos/Skybox/Dia/cara_5.png", "Recursos/Skybox/Dia/cara_6.png"
    };
    std::vector<std::string> carasNoche1 = {
        "Recursos/Skybox/Noche/cara_1.png", "Recursos/Skybox/Noche/cara_2.png",
        "Recursos/Skybox/Noche/cara_3.png",   "Recursos/Skybox/Noche/cara_4.png",
        "Recursos/Skybox/Noche/cara_5.png", "Recursos/Skybox/Noche/cara_6.png"
    };
    std::vector<std::string> carasAtardecer = {
        "Recursos/Skybox/Atardecer/cara_1.png", "Recursos/Skybox/Atardecer/cara_2.png",
        "Recursos/Skybox/Atardecer/cara_3.png",   "Recursos/Skybox/Atardecer/cara_4.png",
        "Recursos/Skybox/Atardecer/cara_5.png", "Recursos/Skybox/Atardecer/cara_6.png"
    };

    // Cargar los cubemaps en el arreglo indexado
    unsigned int cubemaps[3];
    cubemaps[0] = cargarCubemap(carasDia);
    cubemaps[1] = cargarCubemap(carasNoche1);
    cubemaps[2] = cargarCubemap(carasAtardecer);

    // Habilitar estados de OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Ciclo de renderizado principal
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Procesar controles de teclado
        processInput(window, camera, deltaTime);

        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        //  1. RENDERIZAR MODELOS 
    
        nuestroShader.use();
        nuestroShader.setInt("texture_diffuse1", 0);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        nuestroShader.setMat4("projection", projection);
        nuestroShader.setMat4("view", view);

        // Pasar parámetros de iluminación ambiental y de cámara
        nuestroShader.setVec3("lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
        nuestroShader.setVec3("lightColor", glm::vec3(0.6f, 0.7f, 0.85f)); // Azul lunar nítido
        nuestroShader.setVec3("viewPos", camera.Position);

       

        // Enlazar el Cubemap en GL_TEXTURE1 para los reflejos del material de los objetos
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[cubemapActual]);
        nuestroShader.setInt("skybox", 1);
        nuestroShader.setFloat("reflectionFactor", 0.15f); // 15% de reflejo en tus mallas

        // Dibujar Modelo 1
        glm::mat4 modelN1 = glm::mat4(1.0f);
        modelN1 = glm::translate(modelN1, glm::vec3(0.0f, 5.0f, 0.0f));
        modelN1 = glm::scale(modelN1, glm::vec3(1.0f, 1.0f, 1.0f));

        nuestroShader.setMat4("model", modelN1);
        model1.Draw(nuestroShader);


        // =========================================================
        // PASO 2: RENDERIZAR EL SKYBOX COMO FONDO
        // =========================================================
        glDepthFunc(GL_LEQUAL); // Cambiar criterio de profundidad para dibujar al fondo absoluto (Z = 1.0)
        shaderSkybox.use();
        shaderSkybox.setInt("skybox", 0);

        // Quitar la traslación de la matriz de vista convirtiéndola a mat3 y de vuelta a mat4
        // Esto previene que la cámara "se salga" del Cubemap al caminar por el escenario
        glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        shaderSkybox.setMat4("view", skyboxView);
        shaderSkybox.setMat4("projection", projection);

        // Dibujar el cubo usando la textura del Cubemap actual cargada en GL_TEXTURE0
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[cubemapActual]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS); // Restaurar el comportamiento estándar de profundidad para el siguiente frame

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza de recursos del Skybox antes de cerrar
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}