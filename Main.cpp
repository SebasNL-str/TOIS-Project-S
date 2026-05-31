#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Skybox.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Scene.h"
#include "Menu.h"
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

enum class MenuScreen
{
    Main,
    Settings
};

MenuScreen currentMenuScreen = MenuScreen::Main;

bool FileExists(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    return file.good();
}

void ShowMainMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Main;
    menu.SetTitle("T.O.I.S: PROJECT S");
    menu.SetSubtitle("MAIN MENU");
    menu.SetFooter("ESC: MENU  ENTER: SELECT");
    menu.SetItems({
        gameStarted ? "CONTINUE" : "START TOUR",
        "SETTINGS",
        "EXIT"
        });
    menu.SetSelectedIndex(0);
}

void ShowSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug)
{
    currentMenuScreen = MenuScreen::Settings;
    menu.SetTitle("SETTINGS");
    menu.SetSubtitle("SETTINGS");
    menu.SetFooter("ENTER: SWITCH  ESC: BACK");
    menu.SetItems({
        sound.IsAmbientEnabled() ? "AUDIO: ON" : "AUDIO: OFF",
        hitboxDebug ? "HITBOX: ON" : "HITBOX: OFF",
        "BACK"
        });
}

void SetMenuOpen(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool open)
{
    menuOpen = open;
    menu.SetVisible(open);
    firstMouse = true;

    if (open)
    {
        ShowMainMenu(menu);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        sound.StopAmbient();
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (gameStarted)
        {
            sound.PlayAmbientIfEnabled();
        }
    }
}

void processGameplayInput(GLFWwindow* window, bool& flashlightEnabled)
{
    static bool fWasPressed = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    bool fPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fPressed && !fWasPressed)
    {
        flashlightEnabled = !flashlightEnabled;
    }
    fWasPressed = fPressed;
}

void processMenuInput(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug)
{
    static bool escWasPressed = false;
    static bool upWasPressed = false;
    static bool downWasPressed = false;
    static bool enterWasPressed = false;

    bool escPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escPressed && !escWasPressed)
    {
        if (menuOpen && currentMenuScreen == MenuScreen::Settings)
        {
            ShowMainMenu(menu);
        }
        else if (gameStarted)
        {
            SetMenuOpen(window, menu, sound, !menuOpen);
        }
        else
        {
            SetMenuOpen(window, menu, sound, true);
        }
    }
    escWasPressed = escPressed;

    if (!menuOpen)
    {
        return;
    }

    bool upPressed =
        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;

    bool downPressed =
        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

    bool enterPressed =
        glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (upPressed && !upWasPressed)
    {
        menu.MoveSelection(-1);
    }

    if (downPressed && !downWasPressed)
    {
        menu.MoveSelection(1);
    }

    if (enterPressed && !enterWasPressed)
    {
        if (currentMenuScreen == MenuScreen::Main)
        {
            switch (menu.GetSelectedIndex())
            {
            case 0:
                gameStarted = true;
                SetMenuOpen(window, menu, sound, false);
                break;
            case 1:
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(0);
                break;
            case 2:
                glfwSetWindowShouldClose(window, true);
                break;
            default:
                break;
            }
        }
        else if (currentMenuScreen == MenuScreen::Settings)
        {
            int selectedIndex = menu.GetSelectedIndex();
            switch (selectedIndex)
            {
            case 0:
                sound.ToggleAmbient();
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(selectedIndex);
                break;
            case 1:
                hitboxDebug = !hitboxDebug;
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(selectedIndex);
                break;
            case 2:
                ShowMainMenu(menu);
                break;
            default:
                break;
            }
        }
    }

    upWasPressed = upPressed;
    downWasPressed = downPressed;
    enterWasPressed = enterPressed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (menuOpen || !gameStarted)
    {
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main()
{
    // =========================
    // GLFW
    // =========================
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "T.O.I.S: Project S", NULL, NULL);
    if (!window)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // =========================
    // GLEW
    // =========================
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error inicializando GLEW\n";
        return -1;
    }

    // =========================
    // OPENGL STATE
    // =========================
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    Shader shader("default.vert", "default.frag");
    shader.Use();
    shader.SetInt("texture1", 0);

    // Crear skybox
    Shader skyboxShader("skybox.vert", "skybox.frag");
    skyboxShader.Use();
    skyboxShader.SetInt("skybox", 0);

    std::vector<std::string> faces = {
        "Resources/Skybox/Cubemaps/Night/px.png", // px - right
        "Resources/Skybox/Cubemaps/Night/nx.png", // nx - left
        "Resources/Skybox/Cubemaps/Night/py.png", // py - top
        "Resources/Skybox/Cubemaps/Night/ny.png", // ny - bottom
        "Resources/Skybox/Cubemaps/Night/pz.png", // pz - front
        "Resources/Skybox/Cubemaps/Night/nz.png" // nz - back
    };
    Skybox skybox(faces);

    bool hitboxDebug = false;
    bool hitboxC = false;
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

    while (!glfwWindowShouldClose(window))
    {
        int framebufferWidth = SCREEN_WIDTH;
        int framebufferHeight = SCREEN_HEIGHT;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0)
            framebufferWidth = SCREEN_WIDTH;
        if (framebufferHeight <= 0)
            framebufferHeight = SCREEN_HEIGHT;

        glViewport(0, 0, framebufferWidth, framebufferHeight);

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

            if (hitboxC)
            {
                CameraCollider camCollider{ newPos, 0.2f };
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

        glfwSwapBuffers(window);
    }

    sound.StopAmbient();
    glfwTerminate();
    return 0;
}
