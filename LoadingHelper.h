// GL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// std includes
#include <vector>
#include <string>
#include <memory>

// Project includes
#include "LoadingScreen.h"
#include "Shader.h"
#include "SoundManager.h"
#include "Skybox.h"
#include "Mesh.h"
#include "Scene.h"
#include "Menu.h"
#include "Utils.h"

// Estructura contenedora para empaquetar los punteros generados || Pointers packages structure
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

// Ejecuta el bucle secuencial por estados de la pantalla de carga interactiva. || Runs the on-state sequential loop from the interactive loading-screen
// return Retorna una estructura con todos los punteros de los objetos cargados en memoria. || Returns an structure with all memory loaded pointers

GAssets ExecuteInteractiveLoading(GLFWwindow* window, Shader& loadingShader, GLuint barVAO, GLuint barVBO);
