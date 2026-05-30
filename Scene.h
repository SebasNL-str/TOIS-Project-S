#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"
#include "ShadingModel.h"

#include <glm.hpp>



// =========================
// CONFIG
// =========================
constexpr int MAX_LIGHTS = 16;

// =========================
// SCENE OBJECT
// =========================
struct SceneObject
{
    std::shared_ptr<Model> model;

    Transform transform;

    glm::mat4 GetModelMatrix() const {
        return transform.getMatrix(); // tu función que construye la matriz
    }
};

// =========================
// SCENE CLASS
// =========================
class Scene
{
public:
    // =========================
    // SHADING
    // =========================
    void SetShadingModel(ShadingModel model);

    void SetLightSphere(std::shared_ptr<Model> model);

    std::shared_ptr<Model> lightSphere;

    // =========================
    // OBJECTS
    // =========================
    void AddObject(std::shared_ptr<Model> model, const Transform& transform);

    // =========================
    // LIGHTS
    // =========================
    void AddLight(const Light& light);

    // =========================
    // DEBUG LIGHT VISUALIZATION
    // =========================
    // void SetLightDebugModel(std::shared_ptr<Model> model);

    void SetDebugLights(bool enabled) { debugLights = enabled; }

    // =========================
    // RENDER
    // =========================
    void Draw(Shader& shader, Shader& emissiveShader, Camera& camera);

    void DrawHitboxes(Shader& hitboxShader, Camera& camera, const glm::mat4& projection);

    Transform& GetTransform(std::shared_ptr<Model> model) {
        for (auto& obj : objects) {
            if (obj.model == model) return obj.transform;
        }
        throw std::runtime_error("Model not found in scene");
    }

    const std::vector<SceneObject>& GetObjects() const {
        return objects;
    }

private:
    std::vector<SceneObject> objects;
    std::vector<Light> lights;

    ShadingModel shadingModel;

    // =========================
    // DEBUG LIGHTS
    // =========================
    std::shared_ptr<Model> lightDebugModel;
    bool debugLights = true;
};
