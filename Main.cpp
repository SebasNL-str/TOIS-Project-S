#include <iostream>
#include <fstream>
#include <memory>
#include <vector>


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
    GLFWwindow* window = InitWindow(widthR, heightR, "T.O.I.S: Project S");
    if (!window) return -1;

    LoadWindowIcon(window, "Resources/Icons/TOISS.png");

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!InitOpenGLState()) return -1;

    // =========================
    // CORE SYSTEMS
    // =========================
    MenuRenderer menu;
    MenuSettings menuSettings;
    menuSettings.visible = true;
    menuSettings.useBackgroundImage = false;
    menu.Configure(menuSettings);
    ShowMainMenu(menu);

    SoundManager sound("Resources/Sound/ambient.wav");
    SetMenuOpen(window, menu, sound, true);

    Shader shader("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
    shader.Use();
    shader.SetInt("texture1", 0);

    // Crear skybox
    Shader skyboxShader("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");
    skyboxShader.Use();
    skyboxShader.SetInt("skybox", 0);

    std::vector<std::string> faces = {
        "Resources/Skybox/Cubemaps/Night/px.png", // px - right
        "Resources/Skybox/Cubemaps/Night/nx.png", // nx - left
        "Resources/Skybox/Cubemaps/Night/py.png", // py - top
        "Resources/Skybox/Cubemaps/Night/ny.png", // ny - bottom
        "Resources/Skybox/Cubemaps/Night/pz.png", // pz - front
        "Resources/Skybox/Cubemaps/Night/nz.png"  // nz - back
    };
    Skybox skybox(faces);

    bool hitboxDebug = false;
    bool hullDebug = true;
    bool hitboxC = true;
    bool flashlightEnabled = true;

    Shader skyboxSphereShader("Resources/Shaders/skybox_sphere.vert", "Resources/Shaders/skybox_sphere.frag");
    Shader hitboxShader("Resources/Shaders/hitbox.vert", "Resources/Shaders/hitbox.frag");
    Skybox sphereSkybox("Resources/Skybox/Sphere/moonless_golf_4k.png", SkyboxType::Sphere);

    Scene scene;

    Shader emissiveShader("Resources/Shaders/emissive.vert", "Resources/Shaders/emissive.frag");

    SkyboxType activeType = SkyboxType::Cube;

    auto GRGTF = std::make_shared<Model>("Resources/Models/GLTF/Graveyard/Cementerio.gltf");

    // =========================
    // SCENE OBJECTS
    // =========================
    auto sphere = std::make_shared<Model>("Resources/Models/OBJ/sphere.obj");

    if (FileExists("Resources/Models/OBJ/Sphere.obj"))
    {
        scene.SetLightSphere(sphere);
    }
    else
    {
        scene.SetLightSphere(nullptr);
    }

    scene.AddLight({
        LightType::Point,
        {5.0f, 25.0f, 50.0f},
        {0.0f, -1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f},
        10.0f,
        true
        });

    std::size_t flashlightLightIndex = scene.GetLightCount();
    scene.AddLight({
        LightType::Spot,
        camera.GetPosition(),
        camera.GetFront(),
        {1.0f, 0.92f, 0.75f},
        0.0f,
        false
        });

    scene.AddObject(GRGTF, {
        {5.0f, 0.0f, 45.0f},
        {0.0f, 0.0f, 0.0f},
        {0.8f, 0.8f, 0.8f}
        });

    // APARTADO COLLIDERS (SIN TOCAR)
    for (auto& obj : scene.GetObjects())
    {
        // Pasamos el collider de cada modelo a la función que crea el VAO
        SetupMeshCollider(obj.model->collider);
    }

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
