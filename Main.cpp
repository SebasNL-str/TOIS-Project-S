#include <iostream>
#include <memory>
#include "Skybox.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Scene.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Camera camera(glm::vec3(0.0f, 10.0f, 0.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static float lastX = 400, lastY = 300;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

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

    GLFWwindow* window = glfwCreateWindow(1000, 800, "T.O.I.S: Project S", NULL, NULL);
    if (!window)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader shader("Shaders/default.vert", "Shaders/default.frag");

    // Crear skybox
    Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");
    std::vector<std::string> faces = {
        "Resources/Skybox/Cubemaps/Day/cara_1.png", // px - right
        "Resources/Skybox/Cubemaps/Day/cara_2.png", // nx - left
        "Resources/Skybox/Cubemaps/Day/cara_3.png", // py - top
        "Resources/Skybox/Cubemaps/Day/cara_4.png", // ny - bottom
        "Resources/Skybox/Cubemaps/Day/cara_5.png", // pz - front
        "Resources/Skybox/Cubemaps/Day/cara_6.png" //nz - back
    };
    Skybox skybox(faces);

    bool hitboxDebug = false;
    bool hitboxC = false;

    Shader skyboxSphereShader("Resourcs/Shaders/skybox_sphere.vert", "Resources/Shaders/skybox_sphere.frag");
    Shader hitboxShader("Resources/Shaders/hitbox.vert", "Resources/Shaders/hitbox.frag");
    Skybox sphereSkybox("Resources/Skybox/Sphere/moonless_golf_4k.png", SkyboxType::Sphere);

    Scene scene;

    Shader emissiveShader("Resources/Shaders/emissive.vert", "Resources/Shaders/emissive.frag");

    SkyboxType activeType = SkyboxType::Cube;


    auto GRGTF = std::make_shared<Model>("Resources/Models/GLTF/Graveyard/Cementerio.gltf");

    // Light
    // =========================


    // =========================
    // SCENE OBJECTS
    // =========================
    // LUZ PUNTUAL
// ========================
    auto sphere = std::make_shared<Model>("Resources/Models/OBJ/Sphere.obj");
    // scene.SetLightDebugModel(sphere);
    scene.SetLightSphere(sphere);


    scene.AddLight({
        LightType::Point,
        {5.0f, 5.0f, 0.0f},        // posición (arriba)
        {0.0f, -1.0f, 0.0f},       // dirección (hacia abajo)
        {1.0f, 1.0f, 1.0f},        // color blanco
        20.0f                      // intensidad alta para debug
        });


    scene.AddObject(GRGTF, {
{5.0f, 0.0f, 45.0f},
{0.0f, 0.0f, 0.0f},
{0.8f, 0.8f, 0.8f}
        });


    glm::vec3 camPos = camera.GetPosition();

    while (!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, 1000, 800);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glm::vec3 oldPos = camera.GetPosition();

        processInput(window);


        glm::vec3 newPos = camera.GetPosition();

        if (hitboxC) {
            CameraCollider camCollider{ newPos, 0.2f };
            bool blocked = false;
            for (auto& obj : scene.GetObjects()) {
                // Transformar la caja local del modelo al espacio mundial
                BoundingBox worldBox = TransformBoundingBox(obj.model->hitbox, obj.GetModelMatrix());

                // Usar la caja transformada para colisión
                if (CheckCollisionCameraBox(camCollider, worldBox)) {
                    blocked = true;
                    break;
                }
            }

            if (blocked) {
                camera.ForcePosition(oldPos); // revertimos solo si hay colisión
            }
        }

        glfwPollEvents();




        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =========================
        // SKYBOX
        // =========================
        // Aquí decides si activar el modo debug
        bool debugView = true; // cambia a true si quieres probar sin traslación RECOMENDADO


        if (activeType == SkyboxType::Cube) {
            skyboxShader.Use();
            skyboxShader.SetInt("useViewNoTranslation", debugView ? 1 : 0);
            skybox.Draw(skyboxShader, camera, 1000.0f, 800.0f);
        }
        else if (activeType == SkyboxType::Sphere) {
            skyboxSphereShader.Use();
            sphereSkybox.Draw(skyboxSphereShader, camera, 1000, 800);
        }


        // =========================
        // ESCENA
        // =========================


        shader.Use();

        shader.SetFloat("spotCutOff", glm::cos(glm::radians(12.5f)));
        shader.SetFloat("spotOuterCutOff", glm::cos(glm::radians(17.5f)));

        scene.Draw(shader, emissiveShader, camera);

        if (hitboxDebug) {
            hitboxShader.Use();
            scene.DrawHitboxes(hitboxShader,
                camera,
                glm::perspective(glm::radians(camera.GetZoom()),
                    1000.0f / 800.0f,
                    0.1f,
                    100.0f));
        }

        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}