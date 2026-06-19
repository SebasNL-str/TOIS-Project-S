#include "LoadingHelper.h"

// Ejecutar el proceso secuencial de carga de recursos del juego || Execute the sequential game asset loading process
GAssets ExecuteInteractiveLoading(GLFWwindow* window, Shader& loadingShader, GLuint barVAO, GLuint barVBO) {
    GAssets assets;
    int totalAssets = 5;
    int loadedAssets = 0;
    int loadingState = 0;

    // Arreglo con las rutas de las caras del skybox || Array containing the paths of the skybox faces
    std::vector<std::string> faces = {
        "Resources/Skybox/Cubemaps/eskaibocs/px.png", "Resources/Skybox/Cubemaps/eskaibocs/nx.png",
        "Resources/Skybox/Cubemaps/eskaibocs/py.png", "Resources/Skybox/Cubemaps/eskaibocs/ny.png",
        "Resources/Skybox/Cubemaps/eskaibocs/pz.png", "Resources/Skybox/Cubemaps/eskaibocs/nz.png"
    };

    // Bucle principal de la maquinaria de carga || Main loop of the loading machinery
    while (loadingState < totalAssets) {
        if (glfwWindowShouldClose(window)) break;

        // Renderizar fotogramas intermedios para suavizar la animacion || Render intermediate frames to smooth out the animation
        for (int frame = 0; frame < 30; frame++) {
            UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets);
            glfwPollEvents();
            glfwWaitEventsTimeout(0.016);
        }

        // Procesar la carga del recurso según el estado actual || Process asset loading based on current state
        switch (loadingState) {
        case 0:
            // Instanciar el gestor de sonido ambiental || Instantiate the ambient sound manager
            assets.sound = std::make_shared<SoundManager>("Resources/Sound/ambient.wav");
            loadedAssets = 1;
            loadingState = 1;
            break;

        case 1:
            // Instanciar y compilar la coleccion de shaders || Instantiate and compile the shaders collection
            assets.shader = std::make_shared<Shader>("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
            assets.skyboxShader = std::make_shared<Shader>("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");
            assets.skyboxSphereShader = std::make_shared<Shader>("Resources/Shaders/skybox_sphere.vert", "Resources/Shaders/skybox_sphere.frag");
            assets.hitboxShader = std::make_shared<Shader>("Resources/Shaders/hitbox.vert", "Resources/Shaders/hitbox.frag");
            assets.emissiveShader = std::make_shared<Shader>("Resources/Shaders/emissive.vert", "Resources/Shaders/emissive.frag");
            loadedAssets = 2;
            loadingState = 2;
            break;

        case 2:
            // Cargar configuraciones de mapas cubicos y esfericos || Load cubemap and spherical map configurations
            assets.skybox = std::make_shared<Skybox>(faces);
            assets.sphereSkybox = std::make_shared<Skybox>("Resources/Skybox/Sphere/moonless_golf_4k.png", SkyboxType::Sphere);
            loadedAssets = 3;
            loadingState = 3;
            break;

        case 3:
            // Forzar fotogramas adicionales antes del bloqueo de disco || Force additional frames before disk block
            for (int f = 0; f < 45; f++) {
                UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets);
                glfwPollEvents();
                glfwWaitEventsTimeout(0.016);
            }

            // Cargar modelos geometricos tridimensionales pesados || Load heavy three dimensional geometric models
            assets.GYGLTF = std::make_shared<Model>("Resources/Models/GLTF/Graveyard/Cementerio.gltf");
            assets.GYHGLTF = std::make_shared<Model>("Resources/Models/GLTF/Hitbox/Hitbox.gltf");
            assets.sphere = std::make_shared<Model>("Resources/Models/OBJ/sphere.obj");
            loadedAssets = 4;
            loadingState = 4;
            break;

        case 4:
            // Finalizar el conteo de recursos cargados || Finalize the loaded assets count
            loadedAssets = 5;
            loadingState = 5;
            break;
        }
    }

    return assets;
}
