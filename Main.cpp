#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <future>  // Para std::async y std::future
#include <chrono>  // Para manejar tiempos si es necesario

#include "Skybox.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Menu.h"
#include "WindowManager.h"
#include "MenuController.h"
#include "InputHandler.h"
#include "Utils.h"
#include "Scene.h"
#include "SoundManager.h"
#include "LoadingScreen.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 10.0f, 0.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool gameStarted = false;
bool menuOpen = true;
bool firstMouse = true;
float lastX = SCREEN_WIDTH * 0.5f;
float lastY = SCREEN_HEIGHT * 0.5f;

MenuScreen currentMenuScreen = MenuScreen::Main;

int main()
{
    // ==========================================
        // INICIALIZACIÓN DE VENTANA Y ESTADOS (Vía WindowManager)
        // ==========================================
    int widthR, heightR;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = InitWindow(widthR, heightR, "T.O.I.S: Project S");
    if (!window) return -1;

    LoadWindowIcon(window, "Resources/Icons/TOISS.png");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!InitOpenGLState()) return -1;

    Shader loadingShader("Resources/Shaders/loading.vert", "Resources/Shaders/loading.frag");

    GLuint barVAO, barVBO;
    InitProgressBar(barVAO, barVBO);

    int totalAssets = 5;
    int loadedAssets = 0;

    // ============================================================================
    // DECLARACIÓN DE OBJETOS (Asignación diferida, sintaxis original con '.')
    // ============================================================================
    std::shared_ptr<SoundManager> soundPtr;
    std::shared_ptr<Shader> shaderPtr;
    std::shared_ptr<Shader> skyboxShaderPtr;
    std::shared_ptr<Skybox> skyboxPtr;
    std::shared_ptr<Shader> skyboxSphereShaderPtr;
    std::shared_ptr<Shader> hitboxShaderPtr;
    std::shared_ptr<Skybox> sphereSkyboxPtr;
    std::shared_ptr<Shader> emissiveShaderPtr;
    std::shared_ptr<Model> GRGTFPtr;
    std::shared_ptr<Model> spherePtr;

    std::vector<std::string> faces = {
        "Resources/Skybox/Cubemaps/Night/px.png", "Resources/Skybox/Cubemaps/Night/nx.png",
        "Resources/Skybox/Cubemaps/Night/py.png", "Resources/Skybox/Cubemaps/Night/ny.png",
        "Resources/Skybox/Cubemaps/Night/pz.png", "Resources/Skybox/Cubemaps/Night/nz.png"
    };

    // ============================================================================
    // MAQUINARIA DE CARGA INTERACTIVA POR ESTADOS (HILO ÚNICO SEGURO)
    // ============================================================================
    int loadingState = 0;

    // Este bucle procesará un asset a la vez, renderizando los círculos fluidamente entre cargas
    while (loadingState < totalAssets)
    {
        if (glfwWindowShouldClose(window)) return 0;

        // 1. Dibujar la pantalla de carga (Garantiza que los círculos parpadeen en cada iteración)
        UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets);

        // 2. Procesar el asset correspondiente a este frame
        switch (loadingState)
        {
        case 0:
            // Asset 1: Audio
            soundPtr = std::make_shared<SoundManager>("Resources/Sound/ambient.wav");
            loadedAssets = 1;
            loadingState = 1;
            break;

        case 1:
            // Asset 2: Compilación de Shaders de escena
            shaderPtr = std::make_shared<Shader>("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
            skyboxShaderPtr = std::make_shared<Shader>("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");
            skyboxSphereShaderPtr = std::make_shared<Shader>("Resources/Shaders/skybox_sphere.vert", "Resources/Shaders/skybox_sphere.frag");
            hitboxShaderPtr = std::make_shared<Shader>("Resources/Shaders/hitbox.vert", "Resources/Shaders/hitbox.frag");
            emissiveShaderPtr = std::make_shared<Shader>("Resources/Shaders/emissive.vert", "Resources/Shaders/emissive.frag");
            loadedAssets = 2;
            loadingState = 2;
            break;

        case 2:
            // Asset 3: Carga de mapas de cubos/esferas (Skybox)
            skyboxPtr = std::make_shared<Skybox>(faces);
            sphereSkyboxPtr = std::make_shared<Skybox>("Resources/Skybox/Sphere/moonless_golf_4k.png", SkyboxType::Sphere);
            loadedAssets = 3;
            loadingState = 3;
            break;

        case 3:
            // Asset 4: Modelos 3D geométricos
            GRGTFPtr = std::make_shared<Model>("Resources/Models/GLTF/Graveyard/Cementerio.gltf");
            spherePtr = std::make_shared<Model>("Resources/Models/OBJ/sphere.obj");
            loadedAssets = 4;
            loadingState = 4;
            break;

        case 4:
            // Forzamos un renderizado rápido antes de salir del bucle para pintar el 100%
            loadedAssets = 5;
            loadingState = 5;
            break;
        }

        // Permitir que el sistema operativo procese los eventos gráficos de la ventana
        glfwPollEvents();
    }

    // ============================================================================
    // CREACIÓN DE REFERENCIAS CON NOMBRES ORIGINALES (RECONOCE EL '.')
    // ============================================================================
    SoundManager& sound = *soundPtr;
    Shader& shader = *shaderPtr;
    Shader& skyboxShader = *skyboxShaderPtr;
    Skybox& skybox = *skyboxPtr;
    Shader& skyboxSphereShader = *skyboxSphereShaderPtr;
    Shader& hitboxShader = *hitboxShaderPtr;
    Skybox& sphereSkybox = *sphereSkyboxPtr;
    Shader& emissiveShader = *emissiveShaderPtr;

    bool hitboxDebug = false;
    bool hullDebug = true;
    bool hitboxC = true;
    bool flashlightEnabled = true;
    SkyboxType activeType = SkyboxType::Cube;

    auto GRGTF = GRGTFPtr;
    auto sphere = spherePtr;

    // ============================================================================
    // ASIGNACIONES Y CONFIGURACIÓN POST-CARGA
    // ============================================================================
    MenuRenderer menu;
    MenuSettings menuSettings;
    menuSettings.visible = true;
    menuSettings.useBackgroundImage = false;
    menu.Configure(menuSettings);
    ShowMainMenu(menu);
    SetMenuOpen(window, menu, sound, true);

    shader.Use();
    shader.SetInt("texture1", 0);
    skyboxShader.Use();
    skyboxShader.SetInt("skybox", 0);

    Scene scene;

    if (FileExists("Resources/Models/OBJ/Sphere.obj")) {
        scene.SetLightSphere(sphere);
    }
    else {
        scene.SetLightSphere(nullptr);
    }

    scene.AddLight({ LightType::Point, {5.0f, 25.0f, 50.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 10.0f, true });

    std::size_t flashlightLightIndex = scene.GetLightCount();
    scene.AddLight({ LightType::Spot, camera.GetPosition(), camera.GetFront(), {1.0f, 0.92f, 0.75f}, 0.0f, false });

    scene.AddObject(GRGTF, { {5.0f, 0.0f, 45.0f}, {0.0f, 0.0f, 0.0f}, {0.8f, 0.8f, 0.8f} });

    // Asset 5: Armado de colliders (Último paso síncrono)
    for (auto& obj : scene.GetObjects()) {
        SetupMeshCollider(obj.model->collider);
    }

    // Dibujar el estado final al 100% antes de entrar al juego
    UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, 5, totalAssets);


    while (!glfwWindowShouldClose(window))
    {
        int framebufferWidth = SCREEN_WIDTH;
        int framebufferHeight = SCREEN_HEIGHT;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0)
            framebufferWidth = SCREEN_WIDTH;
        if (framebufferHeight <= 0)
            framebufferHeight = SCREEN_HEIGHT;

        glViewport(0, 0, widthR, heightR);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processMenuInput(window, menu, sound, hitboxDebug);

        if (!menuOpen && gameStarted)
        {
            glm::vec3 oldPos = camera.GetPosition();

            processGameplayInput(window, flashlightEnabled);

            glm::vec3 newPos = camera.GetPosition();

            scene.SetLight(flashlightLightIndex, {
                LightType::Spot,
                newPos + camera.GetFront() * 0.25f,
                camera.GetFront(),
                {1.0f, 0.92f, 0.75f},
                flashlightEnabled ? 18.0f : 0.0f
                });

            // APARTADO COLLIDERS (SIN TOCAR)
            /*
            if (hitboxC)
            {
                CameraCollider camCollider{ newPos, 1.0f };
                bool blocked = false;
                for (auto& obj : scene.GetObjects())
                {
                    BoundingBox worldBox = TransformBoundingBox(obj.model->hitbox, obj.GetModelMatrix());

                    if (CheckCollisionCameraBox(camCollider, worldBox))
                    {
                        blocked = true;
                        break;
                    }
                }

                if (blocked)
                {
                    camera.ForcePosition(oldPos);
                }
            }*/
            if (hitboxC)
            {
                CameraCollider camCollider{ newPos, 0.8f };
                bool blocked = false;
                for (auto& obj : scene.GetObjects())
                {
                    if (CheckCollisionSphereMesh(camCollider.position,
                        camCollider.radius,
                        obj.model->collider,
                        obj.GetModelMatrix()))
                    {
                        blocked = true;
                        break;
                    }
                }

                if (blocked)
                {
                    camera.ForcePosition(oldPos);
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (menuOpen || !gameStarted)
        {
            menu.Render(framebufferWidth, framebufferHeight);
            glfwSwapBuffers(window);
            continue;
        }

        // =========================
        // SKYBOX
        // =========================
        if (activeType == SkyboxType::Cube)
        {
            skyboxShader.Use();
            skybox.Draw(skyboxShader, camera, static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight));
        }
        else if (activeType == SkyboxType::Sphere)
        {
            skyboxSphereShader.Use();
            sphereSkybox.Draw(skyboxSphereShader, camera, static_cast<float>(framebufferWidth), static_cast<float>(framebufferHeight));
        }

        // =========================
        // ESCENA
        // =========================
        shader.Use();

        shader.SetFloat("spotCutOff", glm::cos(glm::radians(12.5f)));
        shader.SetFloat("spotOuterCutOff", glm::cos(glm::radians(17.5f)));

        scene.Draw(shader, emissiveShader, camera);

        // APARTADO COLLIDERS (SIN TOCAR)
        /*
        if (hitboxDebug)
        {
            hitboxShader.Use();
            scene.DrawHitboxes(hitboxShader,
                camera,
                glm::perspective(glm::radians(camera.GetZoom()),
                    static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight),
                    0.1f,
                    100.0f));
        }
        */
        if (hitboxDebug) // flag para activar visualización del convex hull
        {
            hitboxShader.Use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
                static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight),
                0.1f,
                100.0f);

            for (auto& obj : scene.GetObjects())
            {
                glm::mat4 modelMatrix = obj.GetModelMatrix();
                DrawMeshCollider(obj.model->collider,
                    hitboxShader,
                    obj.GetModelMatrix(),
                    camera.GetViewMatrix(),
                    projection,
                    glm::vec3(1.0f, 0.0f, 0.0f)); // rojo
            }
        }

        glfwSwapBuffers(window);
    }

    sound.StopAmbient();
    glfwTerminate();
    return 0;
}
