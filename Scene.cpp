#include "Scene.h"
#include "Transform.h"

#include <gtc/matrix_transform.hpp>



// Dibujar las cajas de colision de los objetos en la escena || Draw object collision boxes in the scene
void Scene::DrawHitboxes(Shader& hitboxShader, Camera& camera, const glm::mat4& projection) {
    hitboxShader.Use();
    // Recorrer el contenedor interno de objetos || Loop through the internal objects container
    for (auto& obj : objects) {
        glm::mat4 modelMatrix = obj.transform.getMatrix();
        DrawHitbox(obj.model->hitbox,
            hitboxShader,
            modelMatrix,
            camera.GetViewMatrix(),
            projection,
            glm::vec3(1.0f, 0.0f, 0.0f));
    }
}

// Configurar el modelo de sombreado actual || Set the current shading model
void Scene::SetShadingModel(ShadingModel model)
{
    shadingModel = model;
}

// Registrar un nuevo objeto con sus transformaciones en la escena || Register a new object with its transformations in the scene
void Scene::AddObject(std::shared_ptr<Model> model, const Transform& transform)
{
    objects.push_back({ model, transform });
}

// Establecer la malla para la visualizacion de las luces || Set the mesh for light visualization
void Scene::SetLightSphere(std::shared_ptr<Model> model)
{
    lightSphere = model;
}

// Agregar una fuente de luz configurada a la escena || Add a configured light source to the scene
void Scene::AddLight(const Light& light)
{
    Light l = light;

    // Normalizar vectores de direccion || Normalize direction vectors
    if (l.type == LightType::Directional || l.type == LightType::Spot)
        l.direction = glm::normalize(l.direction);

    lights.push_back(l);
}

// Actualizar los datos de una luz existente mediante su indice || Update an existing light data using its index
void Scene::SetLight(std::size_t index, const Light& light)
{
    if (index >= lights.size())
    {
        return;
    }

    Light l = light;

    // Normalizar vectores de direccion || Normalize direction vectors
    if (l.type == LightType::Directional || l.type == LightType::Spot)
        l.direction = glm::normalize(l.direction);

    lights[index] = l;
}

// Realizar el bucle principal de renderizado de la escena || Perform the main scene rendering loop
void Scene::Draw(Shader& shader, Shader& emissiveShader, Camera& camera, float windowWidth, float windowHeight)
{
    // Calcular matrices de vista y proyeccion perspectiva || Calculate view and perspective projection matrices
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.GetZoom()),
        windowWidth / windowHeight,
        0.1f,
        1000.0f
    );

    // Configurar estados y variables uniformes del shader de iluminacion || Configure states and uniform variables of the lighting shader
    shader.Use();
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("viewPos", camera.GetPosition());

    // Limitar la cantidad maxima de luces procesadas || Limit the maximum amount of lights processed
    int lightCount = (int)lights.size();
    if (lightCount > MAX_LIGHTS)
        lightCount = MAX_LIGHTS;

    shader.SetInt("numLights", lightCount);

    // Transferir datos estructurados de las luces al shader || Transfer structured light data to the shader
    for (int i = 0; i < lightCount; i++)
    {
        const Light& light = lights[i];
        std::string base = "lights[" + std::to_string(i) + "].";

        shader.SetInt(base + "type", (int)light.type);
        shader.SetVec3(base + "position", light.position);
        shader.SetVec3(base + "direction", light.direction);
        shader.SetVec3(base + "color", light.color);
        shader.SetFloat(base + "intensity", light.intensity);
    }

    // Calcular matrices de transformacion y dibujar objetos || Calculate transformation matrices and draw objects
    for (auto& obj : objects)
    {
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, obj.transform.position);

        // Aplicar rotaciones en los ejes X, Y y Z || Apply rotations on X, Y and Z axes
        modelMat = glm::rotate(modelMat,
            glm::radians(obj.transform.rotation.x),
            glm::vec3(1, 0, 0));
        modelMat = glm::rotate(modelMat,
            glm::radians(obj.transform.rotation.y),
            glm::vec3(0, 1, 0));
        modelMat = glm::rotate(modelMat,
            glm::radians(obj.transform.rotation.z),
            glm::vec3(0, 0, 1));

        modelMat = glm::scale(modelMat, obj.transform.scale);

        shader.SetMat4("model", modelMat);
        obj.model->Draw();
    }

    // Configurar shader emisivo para las esferas de luz || Configure emissive shader for light spheres
    emissiveShader.Use();
    emissiveShader.SetMat4("view", view);
    emissiveShader.SetMat4("projection", projection);

    // Dibujar representaciones graficas de los focos de luz || Draw graphical representations of light sources
    for (const Light& light : lights)
    {
        if (lightSphere && light.drawSphere)
        {
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, light.position);
            modelMat = glm::scale(modelMat, glm::vec3(0.2f));

            emissiveShader.SetMat4("model", modelMat);
            emissiveShader.SetVec3("emissiveColor", light.color);
            emissiveShader.SetFloat("intensity", light.intensity);

            lightSphere->Draw();
        }
    }
}


