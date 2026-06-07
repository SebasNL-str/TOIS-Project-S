#include <GL/glew.h> 

#include <vector>
#include <string>
#include <memory>
#include <GLFW/glfw3.h>

// Cabeceras de tu motor#
#include "LoadingScreen.h"
#include "Shader.h"
#include "SoundManager.h"
#include "Skybox.h"
#include "Mesh.h"
#include "Scene.h"
#include "Menu.h"
#include "Utils.h"


// Estructura contenedora para empaquetar los punteros generados
struct GAssets {
    std::shared_ptr<SoundManager> sound;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> skyboxShader;
    std::shared_ptr<Skybox> skybox;
    std::shared_ptr<Shader> skyboxSphereShader;
    std::shared_ptr<Shader> hitboxShader;
    std::shared_ptr<Skybox> sphereSkybox;
    std::shared_ptr<Shader> emissiveShader;
    std::shared_ptr<Model> GYGLTF;
    std::shared_ptr<Model> GYHGLTF;
    std::shared_ptr<Model> sphere;
};

/**
 * @brief Ejecuta el bucle secuencial por estados de la pantalla de carga interactiva.
 * @return Retorna una estructura con todos los punteros de los objetos cargados en memoria.
 */
GAssets ExecuteInteractiveLoading(GLFWwindow* window, Shader& loadingShader, GLuint barVAO, GLuint barVBO);
