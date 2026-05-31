#include "Scene.h"
#include <gtc/matrix_transform.hpp>
#include "Transform.h"

void Scene::DrawHitboxes(Shader& hitboxShader, Camera& camera, const glm::mat4& projection) {
    hitboxShader.Use();
    for (auto& obj : objects) { // aquí sí usas tu vector interno de objetos
        glm::mat4 modelMatrix = obj.transform.getMatrix();
        DrawHitbox(obj.model->hitbox,
            hitboxShader,
            modelMatrix,
            camera.GetViewMatrix(),
            projection,
            glm::vec3(1.0f, 0.0f, 0.0f));
    }
}



void Scene::SetShadingModel(ShadingModel model)
{
    shadingModel = model;
}

void Scene::AddObject(std::shared_ptr<Model> model, const Transform& transform)
{
    objects.push_back({ model, transform });
}

/*
void Scene::SetLightDebugModel(std::shared_ptr<Model> model)
{
    lightDebugModel = model;
}*/

void Scene::SetLightSphere(std::shared_ptr<Model> model)
{
    lightSphere = model;
}


void Scene::AddLight(const Light& light)
{
    Light l = light;

    if (l.type == LightType::Directional || l.type == LightType::Spot)
        l.direction = glm::normalize(l.direction);

    lights.push_back(l);
}

void Scene::SetLight(std::size_t index, const Light& light)
{
    if (index >= lights.size())
    {
        return;
    }

    Light l = light;

    if (l.type == LightType::Directional || l.type == LightType::Spot)
        l.direction = glm::normalize(l.direction);

    lights[index] = l;
}

void Scene::Draw(Shader& shader, Shader& emissiveShader, Camera& camera)
{
    // =========================
    // VIEW / PROJECTION
    // =========================
    glm::mat4 view = camera.GetViewMatrix();

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.GetZoom()),
        800.0f / 600.0f,
        0.1f,
        1000.0f
    );

    // =========================
    // SHADER PRINCIPAL (ILUMINACIÓN)
    // =========================
    shader.Use();

    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);

    shader.SetVec3("viewPos", camera.GetPosition());

    // =========================
    // LIGHTS
    // =========================
    int lightCount = (int)lights.size();
    if (lightCount > MAX_LIGHTS)
        lightCount = MAX_LIGHTS;

    shader.SetInt("numLights", lightCount);

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

    // =========================
    // OBJECTS (ILUMINADOS)
    // =========================
    for (auto& obj : objects)
    {
        glm::mat4 modelMat = glm::mat4(1.0f);

        modelMat = glm::translate(modelMat, obj.transform.position);

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

    // =========================
    // LIGHT VISUALIZATION (EMISSIVE SPHERES)
    // =========================
    emissiveShader.Use();

    emissiveShader.SetMat4("view", view);
    emissiveShader.SetMat4("projection", projection);



    for (const Light& light : lights)
    {
        glm::mat4 modelMat = glm::mat4(1.0f);

        modelMat = glm::translate(modelMat, light.position);
        modelMat = glm::scale(modelMat, glm::vec3(0.2f));

        emissiveShader.SetMat4("model", modelMat);
        emissiveShader.SetVec3("emissiveColor", light.color);
        emissiveShader.SetFloat("intensity", light.intensity);

        if (lightSphere)
        {
            lightSphere->Draw();
        }
    }
}
