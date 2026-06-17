#pragma once

#include <glm.hpp>

#include "Shader.h"

// One fog layer that can be reused for local and far fog.
// Una capa de niebla reutilizable para niebla local y lejana.
struct FogLayer
{
    // Fog tint/color. Lower RGB values make it darker and more horror-like.
    // Tinte/color de la niebla. Valores RGB bajos la hacen mas oscura y tenebrosa.
    glm::vec3 color = glm::vec3(0.34f, 0.38f, 0.40f);

    // Distance where this layer starts and where it reaches its strongest range.
    // Distancia donde esta capa inicia y donde alcanza su rango mas fuerte.
    float nearDistance = 0.5f;
    float farDistance = 10.0f;

    // Layer thickness. Increase for denser fog.
    // Grosor de la capa. Aumentalo para una niebla mas densa.
    float density = 30.0f;

    // Ground level and vertical fade. Use 0.0 heightFalloff for full-height far fog.
    // Nivel del suelo y desvanecido vertical. Usa heightFalloff 0.0 para niebla lejana de altura completa.
    float baseHeight = 1.2f;
    float heightFalloff = 0.75f;

    // Upper opacity limit so the scene remains readable.
    // Limite maximo de opacidad para que la escena siga siendo legible.
    float maxOpacity = 0.78f;
};

// Adjustable fog parameters for the main scene shader.
// Parametros ajustables de niebla para el shader principal de la escena.
struct FogSettings
{
    // Toggle the effect without removing shader code. || Activa o desactiva el efecto sin quitar codigo del shader.
    bool enabled = true;

    // Local/model fog: wraps nearby objects and stays closer to the floor.
    // Niebla local/del modelo: envuelve objetos cercanos y se mantiene mas pegada al suelo.
    FogLayer localLayer;

    // Far boundary fog: closes the background to create an enclosed limit.
    // Niebla lejana de limite: cierra el fondo para crear una sensacion de encierro.
    FogLayer boundaryLayer = {
     glm::vec3(0.24f, 0.27f, 0.29f), // color gris oscuro
     25.0f,                          // empieza a notarse desde esta distancia
     25.0f,                          // llega a su fuerza máxima cerca de esta distancia
     1.0f,                         // densidad/grosor de esa niebla
     0.0f,                           // altura base
     0.0f,                           // sin caída vertical: cubre toda la altura
     0.88f                           // opacidad máxima
    };

    // Send these settings to the currently active shader.
    // Envia estos ajustes al shader que esta activo actualmente.
    void Apply(Shader& shader) const;
};
