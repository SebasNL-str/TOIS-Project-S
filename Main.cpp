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
#include "Bloom.h"

// Dimensiones predeterminadas   de la pantalla || Default screen dimensions
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

struct TourFadeSettings
{
    float durationSeconds = 1.0f;
    glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

TourFadeSettings tourFadeSettings;
bool tourFadeActive = false;
float tourFadeOpacity = 0.0f;

Bloom bloom;
bool bloomEnabled = true;

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
    bloom.Init(widthR, heightR);


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
    menuSettings.useBackgroundImage = true;
    menuSettings.backgroundImagePath = "Resources/MenuBackground/menu2.png";
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
// Multiplicadores de intensidad HDR para forzar el Bloom
    float mainLightIntensity = 10.0f;
    float secondaryLightIntensity = 5.0f;

    glm::vec3 baseLightColor(0.55f, 0.45f, 0.15f);
    glm::vec3 hdrMainColor = baseLightColor * mainLightIntensity;         // Resultado: {5.5f, 4.5f, 1.5f}
    glm::vec3 hdrSecondaryColor = baseLightColor * secondaryLightIntensity; // Resultado: {2.75f, 2.25f, 0.75f}

    // Agregar fuentes de luz puntuales con intensidades HDR corregidas
// El color vuelve a ser el original (rango 0.0 a 1.0)

    // Ajustamos la INTENSIDAD de la estructura para el HDR (Prueba con 3.0f o 4.0f)
    float hdrIntensity = 4.0f;

    scene.AddLight({ LightType::Point, {4.3f, 3.7f, 32.5f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {4.777f, 3.644f, 40.849f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {13.832f, 3.71f, 36.341f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {4.808f, 3.697f, 49.371f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {-3.493f, 3.678f, 48.886f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {-3.541f, 3.723f, 35.901f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {10.843f, 3.689f, 56.531f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {5.083f, 3.723f, 63.825f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {4.991f, 3.723f, 75.983f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {4.79f, 3.726f, 85.209f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {-3.427f, 3.676f, 77.844f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    scene.AddLight({ LightType::Point, {-3.427f, 3.676f, 76.235f}, {0.0f, -1.0f, 0.0f}, baseLightColor, hdrIntensity, true });
    
    /*
    scene.AddLight({ LightType::Point, {-1.5f, 2.5f, 62.5f}, {0.0f, -1.0f, 0.0f}, hdrSecondaryColor, 1.0f, false });
    scene.AddLight({ LightType::Point, {10.5f, 2.5f, 62.5f}, {0.0f, -1.0f, 0.0f}, hdrSecondaryColor, 1.0f, false });
    scene.AddLight({ LightType::Point, {10.5f, 2.5f, 25.0f}, {0.0f, -1.0f, 0.0f}, hdrSecondaryColor, 1.0f, false });
    scene.AddLight({ LightType::Point, {-1.5f, 2.5f, 25.0f}, {0.0f, -1.0f, 0.0f}, hdrSecondaryColor, 1.0f, false });*/

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
    PlayLoadingFadeOut(window, barVAO, barVBO, loadingShader);
    menu.StartIntroAnimation();



    // =========================================================================
        // INICIO DEL BUCLE DE RENDERIZADO PRINCIPAL CORREGIDO
        // =========================================================================
    while (!glfwWindowShouldClose(window))
    {
        // Actualizar información de rendimiento en la ventana (FPS)
        updateFPS(window);

        // Inicializar variables para dimensiones del buffer de la ventana
        int framebufferWidth = SCREEN_WIDTH;
        int framebufferHeight = SCREEN_HEIGHT;

        // Obtener dimensiones reales del buffer de la ventana (Crucial para pantallas Retina/High-DPI)
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0) framebufferWidth = SCREEN_WIDTH;
        if (framebufferHeight <= 0) framebufferHeight = SCREEN_HEIGHT;

        glm::mat4 globalProjection = glm::perspective(
            glm::radians(camera.GetZoom()),
            (float)framebufferWidth / (float)framebufferHeight,
            0.1f,
            100.0f
        );

        // Establecer de forma dinámica las dimensiones del área de dibujo de OpenGL
        glViewport(0, 0, framebufferWidth, framebufferHeight);

        // Calcular el tiempo transcurrido entre fotogramas (Delta Time)
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Lógica de transición / Fade del tour
        if (tourFadeActive) {
            if (tourFadeSettings.durationSeconds <= 0.0f) {
                tourFadeOpacity = 0.0f;
                tourFadeActive = false;
            }
            else {
                tourFadeOpacity -= deltaTime / tourFadeSettings.durationSeconds;
                if (tourFadeOpacity <= 0.0f) {
                    tourFadeOpacity = 0.0f;
                    tourFadeActive = false;
                }
            }
        }

        // Capturar eventos del sistema (Teclado, Mouse, Ventana)
        glfwPollEvents();

        // Procesar entradas del menú principal o de pausa
        processMenuInput(window, menu, sound, hitboxDebug);

        // Procesar lógica y controles del gameplay únicamente si el menú está cerrado y el juego inició
        if (!menuOpen && gameStarted) {
            glm::vec3 oldPos = camera.GetPosition();
            processGameplayInput(window, flashlightEnabled, bloomEnabled);
            glm::vec3 newPos = camera.GetPosition();

            // Actualizar coordenadas, dirección y estado de la linterna acoplada a la cámara
            scene.SetLight(flashlightLightIndex, {
                LightType::Spot,
                newPos + camera.GetFront() * 0.25f,
                camera.GetFront(),
                {1.0f, 0.92f, 0.75f},
                flashlightEnabled ? 18.0f : 0.0f
                });

            // Evaluar colisiones por malla del sistema nativo
            if (hitboxC) {
                CameraCollider camCollider{ newPos, 0.8f };
                bool blocked = false;
                for (auto& obj : scene.GetObjects()) {
                    if (CheckCollisionSphereMesh(camCollider.position,
                        camCollider.radius,
                        obj.model->collider,
                        obj.GetModelMatrix())) {
                        blocked = true;
                        break;
                    }
                }
                if (blocked) camera.ForcePosition(oldPos); // Revertir movimiento si colisiona
            }
        }

        // =========================================================================
        // PASO 1: Redirigir el Renderizado de la Escena al HDR FBO de Bloom
        // =========================================================================
        glBindFramebuffer(GL_FRAMEBUFFER, bloom.hdrFBO);

        // Forzar encendido del Test de Profundidad antes de limpiar el buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND); // Empezamos sin transparencias parasitarias del frame anterior

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Decidir qué renderizar estructuralmente dentro del FBO sin romper el ciclo
        if (!gameStarted) {
            // --- ESTADOS PARA INTERFAZ 2D DEL MENÚ INICIAL ---
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST); // Evita que la UI se oculte detrás del z-buffer inexistente

            menu.Render(framebufferWidth, framebufferHeight);

            // Limpieza inmediata de estados tras renderizar el menú
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glBindVertexArray(0);
        }
        else {
            // --- ESTADOS PARA MUNDO 3D ACTIVO ---
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            // -----------------------------------------------------------------
            // 1. Dibujar el cielo de fondo (Skybox) con test inclusivo LEQUAL
            // -----------------------------------------------------------------
            glDepthFunc(GL_LEQUAL); // Permite dibujar fragmentos en el infinito profundo (Z = 1.0)

            if (activeType == SkyboxType::Cube) {
                skyboxShader.Use();
                skybox.Draw(skyboxShader, camera, (float)framebufferWidth, (float)framebufferHeight);
            }
            else if (activeType == SkyboxType::Sphere) {
                skyboxSphereShader.Use();
                sphereSkybox.Draw(skyboxSphereShader, camera, (float)framebufferWidth, (float)framebufferHeight);
            }

            // Regresar inmediatamente a GL_LESS antes de pintar los objetos del escenario
            glDepthFunc(GL_LESS);

            // -----------------------------------------------------------------
            // 2. Activar el shader de los objetos e iluminar los límites de la linterna (Spotlight)
            // -----------------------------------------------------------------
            shader.Use();
            shader.SetFloat("spotCutOff", glm::cos(glm::radians(12.5f)));
            shader.SetFloat("spotOuterCutOff", glm::cos(glm::radians(17.5f)));

            // 3. Dibujar todos los elementos opacos y emisivos del escenario
            scene.Draw(shader, emissiveShader, camera, globalProjection);
            glBindVertexArray(0);

            // 4. Renderizar líneas de depuración de colisiones (Hitboxes) en caso de estar activo
            if (hitboxDebug) {
                hitboxShader.Use();
                glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
                    (float)framebufferWidth / (float)framebufferHeight, 0.1f, 100.0f);
                for (auto& obj : scene.GetObjects()) {
                    DrawMeshCollider(obj.model->collider,
                        hitboxShader,
                        obj.GetModelMatrix(),
                        camera.GetViewMatrix(),
                        projection,
                        glm::vec3(1.0f, 0.0f, 0.0f));
                }
                glBindVertexArray(0);
            }

            // 5. Aplicar la capa/overlay de la transición del Tour si corresponde
            if (tourFadeOpacity > 0.0f) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_DEPTH_TEST);

                glm::vec4 fadeColor = tourFadeSettings.color;
                fadeColor.a *= tourFadeOpacity;
                menu.RenderOverlay(framebufferWidth, framebufferHeight, fadeColor);

                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                glBindVertexArray(0);
            }

            // 6. Si el juego está activo pero abriste el menú de pausa, lo superponemos en el FBO
            if (menuOpen) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_DEPTH_TEST);

                menu.Render(framebufferWidth, framebufferHeight);

                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                glBindVertexArray(0);
            }
        }

        // =========================================================================
        // PASO 2: Desvincular el HDR FBO y Ejecutar el Post-Procesado de Bloom
        // =========================================================================
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Regresamos al buffer por defecto del monitor

        // AISLAMIENTO TOTAL: Forzar la limpieza de estados antes del dibujo del Quad de Bloom
        glUseProgram(0);           // Matar cualquier shader activo residual (ej: fuentes/UI del menú)
        glBindVertexArray(0);      // Asegurar que no haya VAO enlazado incorrectamente
        glDisable(GL_BLEND);       // El quad de Bloom debe sobreescribir la pantalla de manera 100% opaca
        glDisable(GL_DEPTH_TEST);  // Apagar profundidad porque el Quad de post-procesado es 2D puro

        // Procesar texturas: Extraer brillos -> Aplicar Blur -> Combinar aditivamente en pantalla
        bloom.Render(bloomEnabled);

        // Intercambiar los buffers de dibujo para mostrar el fotograma final estable en el monitor
        glfwSwapBuffers(window);
    }
    // =========================================================================
    // FIN DEL BUCLE DE RENDERIZADO PRINCIPAL
    // =========================================================================



    sound.StopAmbient();
    glfwTerminate();
    return 0;
}
