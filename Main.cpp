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

    // Directional light
    scene.AddLight({ LightType::Directional, {0.0f, 0.0f, 0.0f}, {-0.2f, -1.0f, -0.3f}, {0.45f, 0.55f, 0.70f}, 1.2f, false });


    std::size_t flashlightLightIndex = scene.GetLightCount();
    scene.AddLight({ LightType::Spot, camera.GetPosition(), camera.GetFront(), {1.0f, 0.92f, 0.75f}, 0.0f, false });

    scene.AddObject(GRGTF, { {5.0f, 0.0f, 45.0f}, {0.0f, 0.0f, 0.0f}, {0.8f, 0.8f, 0.8f} });

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
