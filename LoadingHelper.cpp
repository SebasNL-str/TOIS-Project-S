#include "LoadingHelper.h" // Asegúrate de que tenga el orden correcto de GLEW antes de GLFW

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

    // ============================================================================
    // MAQUINARIA DE CARGA FLUIDA CENTRALIZADA
    // ============================================================================
    while (loadingState < totalAssets) {
        if (glfwWindowShouldClose(window)) break;

        // TRUCO VISUAL: Añadimos un micro-bucle de fotogramas vacíos ANTES de cada carga.
        // Esto obliga a la tarjeta gráfica a procesar 30 fotogramas de la animación 
        // de los círculos de forma ultra fluida a 60 FPS antes de bloquear la CPU con el disco.
        for (int frame = 0; frame < 30; frame++) {
            // Lee el tiempo global continuo actualizado de GLFW
            UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets);
            glfwPollEvents();
            
            // Un pequeño retraso para controlar la velocidad del refresco (aprox 60fps)
            glfwWaitEventsTimeout(0.016); 
        }

        // Una vez que el spinner giró libremente por un instante, procesamos el asset correspondiente
        switch (loadingState) {
            case 0:
                assets.sound = std::make_shared<SoundManager>("Resources/Sound/ambient.wav");
                loadedAssets = 1; 
                loadingState = 1;
                break;

            case 1:
                assets.shader = std::make_shared<Shader>("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
                assets.skyboxShader = std::make_shared<Shader>("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");
                assets.skyboxSphereShader = std::make_shared<Shader>("Resources/Shaders/skybox_sphere.vert", "Resources/Shaders/skybox_sphere.frag");
                assets.hitboxShader = std::make_shared<Shader>("Resources/Shaders/hitbox.vert", "Resources/Shaders/hitbox.frag");
                assets.emissiveShader = std::make_shared<Shader>("Resources/Shaders/emissive.vert", "Resources/Shaders/emissive.frag");
                loadedAssets = 2; 
                loadingState = 2;
                break;

            case 2:
                assets.skybox = std::make_shared<Skybox>(faces);
                assets.sphereSkybox = std::make_shared<Skybox>("Resources/Skybox/Sphere/moonless_golf_4k.png", SkyboxType::Sphere);
                loadedAssets = 3; 
                loadingState = 3;
                break;

            case 3:
                // Justo antes del Cementerio (que es el peor bloqueo), forzamos más fotogramas 
                // para que el usuario vea girar bien los círculos antes de la pausa de lectura
                for (int f = 0; f < 45; f++) {
                    UpdateLoadingScreen(window, barVAO, barVBO, loadingShader, loadedAssets, totalAssets);
                    glfwPollEvents();
                    glfwWaitEventsTimeout(0.016);
                }
                
                assets.GYGLTF = std::make_shared<Model>("Resources/Models/GLTF/Graveyard/Cementerio.gltf");
                assets.GYHGLTF = std::make_shared<Model>("Resources/Models/GLTF/Hitbox/Hitbox.gltf");
                assets.sphere = std::make_shared<Model>("Resources/Models/OBJ/sphere.obj");
                loadedAssets = 4; 
                loadingState = 4;
                break;

            case 4:
                loadedAssets = 5; 
                loadingState = 5;
                break;
        }
    }

    return assets;
}
