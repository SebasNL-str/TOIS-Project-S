#pragma once

// std includes
#include <vector>
#include <memory>
#include <iostream>

// glm include
#include <glm.hpp>

// Project includes
#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"
#include "ShadingModel.h"
#include "FogSettings.h"

// Numero maximo de luces permitidas || Maximum number of lights allowed
constexpr int MAX_LIGHTS = 16;

// Estructura para objetos dentro de la escena || Structure for objects within the scene
struct SceneObject
{
    std::shared_ptr<Model> model;

    Transform transform;

    // Obtener la matriz de transformacion del modelo || Get the model transformation matrix
    glm::mat4 GetModelMatrix() const {
        return transform.getMatrix();
    }
};

// Clase principal para la gestion de la escena 3D || Main class for 3D scene management
class Scene
{
public:
    // Cambiar el modelo de sombreado actual || Change the current shading model
    void SetShadingModel(ShadingModel model);

    // Asignar el modelo visual para representar las luces || Assign the visual model to represent lights
    void SetLightSphere(std::shared_ptr<Model> model);

    std::shared_ptr<Model> lightSphere;

    // Añadir un nuevo objeto con su transformacion || Add a new object with its transformation
    void AddObject(std::shared_ptr<Model> model, const Transform& transform);

    // Añadir una nueva luz a la escena || Add a new light to the scene
    void AddLight(const Light& light);

    // Modificar una luz existente por su indice || Modify an existing light by its index
    void SetLight(std::size_t index, const Light& light);

    // Activar o desactivar visualizacion de luces || Enable or disable light visualization
    void SetDebugLights(bool enabled) { debugLights = enabled; }

    // Ajustar la niebla de la escena || Adjust scene fog
    void SetFogSettings(const FogSettings& settings) { fogSettings = settings; }

    // Obtener configuracion editable de la niebla || Get editable fog settings
    FogSettings& GetFogSettings() { return fogSettings; }

    // Obtener configuracion de solo lectura de la niebla || Get read-only fog settings
    const FogSettings& GetFogSettings() const { return fogSettings; }

    // Renderizar todos los elementos de la escena || Render all elements of the scene
    void Draw(Shader& shader, Shader& emissiveShader, Camera& camera, float windowWidth, float windowHeight);

    // Renderizar las cajas de colision de los objetos || Render object collision boxes
    void DrawHitboxes(Shader& hitboxShader, Camera& camera, const glm::mat4& projection);

    // Obtener la referencia de transformacion de un modelo || Get the transformation reference of a model
    Transform& GetTransform(std::shared_ptr<Model> model) {
        for (auto& obj : objects) {
            if (obj.model == model) return obj.transform;
        }
        throw std::runtime_error("Model not found in scene");
    }

    // Obtener lista de objetos de la escena || Get list of scene objects
    const std::vector<SceneObject>& GetObjects() const {
        return objects;
    }

    // Obtener la cantidad total de luces || Get total light count
    std::size_t GetLightCount() const {
        return lights.size();
    }

private:
    std::vector<SceneObject> objects;
    std::vector<Light> lights;

    ShadingModel shadingModel;
    FogSettings fogSettings;

    std::shared_ptr<Model> lightDebugModel;
    bool debugLights = true;
};

