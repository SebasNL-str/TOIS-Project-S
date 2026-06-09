#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <future> 
#include <chrono>  

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
#include "LoadingHelper.h"

// Dimensiones predeterminadas de la pantalla || Default screen dimensions
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

// Inicializacion de la camara en el espacio || Camera initialization in space
Camera camera(glm::vec3(3.0f, 1.0f, 35.0f));

// Variables globales para el control del tiempo delta || Global variables for delta time control
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Flags de estado del bucle de juego y comportamiento del mouse || State flags for game loop and mouse behavior
bool gameStarted = false;
bool menuOpen = true;
bool firstMouse = true;
float lastX = SCREEN_WIDTH * 0.5f;
float lastY = SCREEN_HEIGHT * 0.5f;

// Estado de la pantalla activa del menu || Active menu screen state
MenuScreen currentMenuScreen = MenuScreen::Main;

int main()
{
    int widthR, heightR;
    // Inicializar ventana principal del sistema || Initialize main system window
    GLFWwindow* window = InitWindow(widthR, heightR, "T.O.I.S: Project S");
    if (!window) return -1;

    // Configurar recursos iniciales e interrupciones de la ventana || Configure initial window resources and callbacks
    LoadWindowIcon(window, "Resources/Icons/TOISS.png");
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!InitOpenGLState()) return -1;

    // Inicializar el shader y buffers de la interfaz de carga || Initialize loading interface shader and buffers
    Shader loadingShader("Resources/Shaders/loading.vert", "Resources/Shaders/loading.frag");
    GLuint barVAO, barVBO;
    InitProgressBar(barVAO, barVBO);

    // Cargar asincronamente los recursos mediante la maquina de estados || Asynchronously load resources using the state machine
    GAssets loadedData = ExecuteInteractiveLoading(window, loadingShader, barVAO, barVBO);

    // Desreferenciar objetos cargados para compatibilidad de sintaxis || Dereference loaded objects for syntax compatibility
    SoundManager& sound = *loadedData.sound;
    Shader& shader = *loadedData.shader;
    Shader& skyboxShader = *loadedData.skyboxShader;
    Skybox& skybox = *loadedData.skybox;
    Shader& skyboxSphereShader = *loadedData.skyboxSphereShader;
    Shader& hitboxShader = *loadedData.hitboxShader;
    Skybox& sphereSkybox = *loadedData.sphereSkybox;
    Shader& emissiveShader = *loadedData.emissiveShader;

    auto GRGTF = loadedData.GYGLTF;
    auto GRGTF_Collider = loadedData.GYHGLTF;
    auto sphere = loadedData.sphere;
	auto Lantern = loadedData.Lantern;
    auto Lantern2 = std::make_shared<Model>(*Lantern);
    auto Lantern3 = std::make_shared<Model>(*Lantern);
    auto Lantern4 = std::make_shared<Model>(*Lantern);
    auto Lantern5 = std::make_shared<Model>(*Lantern);
    auto Lantern6 = std::make_shared<Model>(*Lantern);
    auto Lantern7 = std::make_shared<Model>(*Lantern);
    auto Lantern8 = std::make_shared<Model>(*Lantern);




    // Definir estados booleanos de simulacion original || Define original simulation boolean states
    bool hitboxDebug = false;
    bool hullDebug = true;
    bool hitboxC = true;
    bool flashlightEnabled = true;
    SkyboxType activeType = SkyboxType::Cube;

    // Configurar e inicializar el estado del menu principal || Configure and initialize main menu state
    MenuRenderer menu;
    MenuSettings menuSettings;
    menuSettings.visible = true;
    menuSettings.useBackgroundImage = false;
    menu.Configure(menuSettings);
    ShowMainMenu(menu);
    SetMenuOpen(window, menu, sound, true);

    // Vincular unidades de textura iniciales en los shaders || Bind initial texture units within shaders
    shader.Use();
    shader.SetInt("texture1", 0);
    skyboxShader.Use();
    skyboxShader.SetInt("skybox", 0);

    // Instanciar escena y transferir mallas de colision || Instantiate scene and transfer collision meshes
    Scene scene;
    GRGTF->collider = GRGTF_Collider->collider;

    // Validar existencia de archivos para representacion de luces || Validate file existence for light representations
    if (FileExists("Resources/Models/OBJ/Sphere.obj")) {
        scene.SetLightSphere(sphere);
    }
    else {
        scene.SetLightSphere(nullptr);
    }

    // Agregar fuentes de luz puntuales y focales a la escena || Add point and spot light sources to the scene
    // scene.AddLight({ LightType::Point, {5.0f, 25.0f, 50.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 10.0f, true });
    scene.AddLight({ LightType::Point, {0.0f, 2.5f, 35.0f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 2.5f, false });
    scene.AddLight({ LightType::Point, {10.0f, 2.5f, 40.0f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 2.5f, false });
    scene.AddLight({ LightType::Point, {10.0f, 2.5f, 50.0f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 2.5f, false });
    scene.AddLight({ LightType::Point, {-3.5f, 2.5f, 59.5f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 2.5f, false });
    scene.AddLight({ LightType::Point, {-1.5f, 2.5f, 62.5f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 1.0f, false });
    scene.AddLight({ LightType::Point, {10.5f, 2.5f, 62.5f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 1.0f, false });
    scene.AddLight({ LightType::Point, {10.5f, 2.5f, 25.0f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 1.0f, false });
    scene.AddLight({ LightType::Point, {-1.5f, 2.5f, 25.0f}, {0.0f, -1.0f, 0.0f}, {0.55f, 0.45f, 0.15f}, 1.0f, false });

    // Directional light
    scene.AddLight({ LightType::Directional, {0.0f, 0.0f, 0.0f}, {-0.2f, -1.0f, -0.3f}, {0.45f, 0.55f, 0.70f}, 1.2f, false });

    std::size_t flashlightLightIndex = scene.GetLightCount();
    scene.AddLight({ LightType::Spot, camera.GetPosition(), camera.GetFront(), {1.0f, 0.92f, 0.75f}, 0.0f, false });

    scene.AddObject(GRGTF, { {5.0f, 0.0f, 45.0f}, {0.0f, 0.0f, 0.0f}, {0.8f, 0.8f, 0.8f} });
    scene.AddObject(Lantern, { {0.0f, 0.0f, 35.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform = scene.GetTransform(Lantern);
    lanternTransform.uniformScale(0.0015f);

    scene.AddObject(Lantern2, { {10.0f, 0.0f, 40.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform2 = scene.GetTransform(Lantern2);
    lanternTransform2.uniformScale(0.0015f);

    scene.AddObject(Lantern3, { {10.0f, 0.0f, 50.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform3 = scene.GetTransform(Lantern3);
    lanternTransform3.uniformScale(0.0015f);


    scene.AddObject(Lantern4, { {-3.5f, 0.0f, 59.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform4 = scene.GetTransform(Lantern4);
    lanternTransform4.uniformScale(0.0015f);

    scene.AddObject(Lantern5, { {-1.5f, 0.0f, 62.5f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform5 = scene.GetTransform(Lantern5);
    lanternTransform5.uniformScale(0.0015f);

    scene.AddObject(Lantern6, { {10.5f, 0.0f, 62.5f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform6 = scene.GetTransform(Lantern6);
    lanternTransform6.uniformScale(0.0015f);

    scene.AddObject(Lantern7, { {10.5f, 0.0f, 25.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform7 = scene.GetTransform(Lantern7);
    lanternTransform7.uniformScale(0.0015f);

    scene.AddObject(Lantern8, { {-1.5f, 0.0f, 25.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} });

    Transform& lanternTransform8 = scene.GetTransform(Lantern8);
    lanternTransform8.uniformScale(0.0015f);




    // Construir estructuras nativas de colision por cada objeto || Build native collision structures for each object
    for (auto& obj : scene.GetObjects()) {
        SetupMeshCollider(obj.model->collider);
    }

    // Refrescar pantalla de carga al valor maximo oficial || Refresh loading screen to official maximum value
    UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, 5, 5);



    while (!glfwWindowShouldClose(window))
    {
        // Actualizar informacion de rendimiento en la ventana || Update performance information on the window
        updateFPS(window);
        int framebufferWidth = SCREEN_WIDTH;
        int framebufferHeight = SCREEN_HEIGHT;

        // Obtener dimensiones reales del buffer de la ventana || Get actual window buffer dimensions
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0)
            framebufferWidth = SCREEN_WIDTH;
        if (framebufferHeight <= 0)
            framebufferHeight = SCREEN_HEIGHT;

        // Establecer las dimensiones del area de dibujo de OpenGL || Set OpenGL drawing area dimensions
        glViewport(0, 0, widthR, heightR);

        // Calcular el tiempo transcurrido entre fotogramas || Calculate time elapsed between frames
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Capturar eventos del sistema y procesar entradas del menu || Capture system events and process menu inputs
        glfwPollEvents();
        processMenuInput(window, menu, sound, hitboxDebug);

        // Procesar logica del juego si el menu esta cerrado || Process game logic if the menu is closed
        if (!menuOpen && gameStarted)
        {
            glm::vec3 oldPos = camera.GetPosition();

            processGameplayInput(window, flashlightEnabled);

            glm::vec3 newPos = camera.GetPosition();

            // Actualizar coordenadas y estado de la linterna de la camara || Update camera flashlight coordinates and state
            scene.SetLight(flashlightLightIndex, {
                LightType::Spot,
                newPos + camera.GetFront() * 0.25f,
                camera.GetFront(),
                {1.0f, 0.92f, 0.75f},
                flashlightEnabled ? 18.0f : 0.0f
                });

            // Evaluar sistema nativo de colisiones por malla || Evaluate native mesh collision system
            if (hitboxC)
            {
                CameraCollider camCollider{ newPos, 0.8f };
                bool blocked = false;

                // Comprobar intersecciones con los objetos de la escena || Check intersections with scene objects
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

                // Revertir posicion en caso de colision detectada || Revert position in case of collision detected
                if (blocked)
                {
                    camera.ForcePosition(oldPos);
                }
            }
        }

        // Limpiar buffers de color y profundidad de la pantalla || Clear screen color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Renderizar unicamente la interfaz si el juego esta en pausa || Render only the interface if the game is paused
        if (menuOpen || !gameStarted)
        {
            menu.Render(framebufferWidth, framebufferHeight);
            glfwSwapBuffers(window);
            continue;
        }

        // Dibujar el cielo de fondo segun la proyeccion activa || Draw background skybox based on active projection
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

        // Activar el shader principal y enviar limites focales || Activate main shader and send spot limits
        shader.Use();

        shader.SetFloat("spotCutOff", glm::cos(glm::radians(12.5f)));
        shader.SetFloat("spotOuterCutOff", glm::cos(glm::radians(17.5f)));

        // Dibujar todos los elementos tridimensionales e iluminacion || Draw all three dimensional elements and lighting
        scene.Draw(shader, emissiveShader, camera, widthR, heightR);

        // Renderizar lineas de depuracion de las mallas de colision || Render debug lines of the collision meshes
        if (hitboxDebug)
        {
            hitboxShader.Use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
                static_cast<float>(widthR) / static_cast<float>(heightR),
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
                    glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        // Refrescar y alternar buffers de dibujo de la ventana || Refresh and swap window drawing buffers
        glfwSwapBuffers(window);
    }


    sound.StopAmbient();
    glfwTerminate();
    return 0;
}
