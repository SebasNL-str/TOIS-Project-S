#include "LoadingHelper.h"

GAssets ExecuteInteractiveLoading(GLFWwindow* window, Shader& loadingShader, GLuint barVAO, GLuint barVBO) {
    GAssets assets;
    int totalAssets = 5;
    int loadedAssets = 0;
    int loadingState = 0;

    std::vector<std::string> faces = {
        "Resources/Skybox/Cubemaps/Night/px.png", "Resources/Skybox/Cubemaps/Night/nx.png",
        "Resources/Skybox/Cubemaps/Night/py.png", "Resources/Skybox/Cubemaps/Night/ny.png",
        "Resources/Skybox/Cubemaps/Night/pz.png", "Resources/Skybox/Cubemaps/Night/nz.png"
    };

    while (loadingState < totalAssets) {
        if (glfwWindowShouldClose(window)) break;

        // Renderizar la animación del spinner independiente de la GPU en cada frame
        UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets);

        switch (loadingState) {
        case 0:
            assets.sound = std::make_shared<SoundManager>("Resources/Sound/ambient.wav");
            loadedAssets = 1; loadingState = 1;
            break;
        case 1:
            assets.shader = std::make_shared<Shader>("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
            assets.skyboxShader = std::make_shared<Shader>("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");
            assets.skyboxSphereShader = std::make_shared<Shader>("Resources/Shaders/skybox_sphere.vert", "Resources/Shaders/skybox_sphere.frag");
            assets.hitboxShader = std::make_shared<Shader>("Resources/Shaders/hitbox.vert", "Resources/Shaders/hitbox.frag");
            assets.emissiveShader = std::make_shared<Shader>("Resources/Shaders/emissive.vert", "Resources/Shaders/emissive.frag");
            loadedAssets = 2; loadingState = 2;
            break;
        case 2:
            assets.skybox = std::make_shared<Skybox>(faces);
            assets.sphereSkybox = std::make_shared<Skybox>("Resources/Skybox/Sphere/moonless_golf_4k.png", SkyboxType::Sphere);
            loadedAssets = 3; loadingState = 3;
            break;
        case 3:
            assets.GYGLTF = std::make_shared<Model>("Resources/Models/GLTF/Graveyard/Cementerio.gltf");
            assets.sphere = std::make_shared<Model>("Resources/Models/OBJ/sphere.obj");
            loadedAssets = 4; loadingState = 4;
            break;
        case 4:
            loadedAssets = 5; loadingState = 5;
            break;
        }
        glfwPollEvents();
    }
    return assets;
}