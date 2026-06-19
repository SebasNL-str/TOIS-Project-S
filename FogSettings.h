#pragma once

#include <glm.hpp>

#include "Shader.h"

// One fog layer that can be reused for local and far fog.
// Una capa de niebla reutilizable para niebla local y lejana.
struct FogLayer
{
    // Fog tint/color. Lower RGB values make it darker and more horror-like.
    // Tinte/color de la niebla. Valores RGB bajos la hacen mas oscura y tenebrosa.
    glm::vec3 color = glm::vec3(0.10f, 0.12f, 0.13f);

    // Distance where this layer starts and where it reaches its strongest range.
    // Distancia donde esta capa inicia y donde alcanza su rango mas fuerte.
    float nearDistance = 4.0f;
    float farDistance = 18.0f;

    // Layer thickness. Increase for denser fog.
    // Grosor de la capa. Aumentalo para una niebla mas densa.
    float density = 0.12f;

    // Ground level and vertical fade. Use 0.0 heightFalloff for full-height far fog.
    // Nivel del suelo y desvanecido vertical. Usa heightFalloff 0.0 para niebla lejana de altura completa.
    float baseHeight = 10.0f;
    float heightFalloff = 0.08f;

    // Upper opacity limit so the scene remains readable.
    // Limite maximo de opacidad para que la escena siga siendo legible.
    float maxOpacity = 0.35f;

    // Full-hide threshold. Use more than 1.0 to disable hard occlusion on this layer.
    // Umbral de ocultacion total. Usa mas de 1.0 para desactivar el corte solido en esta capa.
    float solidCutoff = 1.01f;
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
     glm::vec3(0.025f, 0.030f, 0.035f), // color casi igual al skybox oscuro
     16.0f,                             // empieza a cerrar la vista desde esta distancia
     28.0f,                             // a esta distancia ya debe ocultar como niebla densa
     0.18f,                             // densidad/grosor de esa niebla
     30.0f,                             // altura alta para cubrir objetos completos
     0.0f,                              // sin caida vertical: cubre toda la altura
     1.0f,                              // opacidad maxima real
     0.98f                              // corte solido: elimina textura/luz al estar cubierto
    };

    // Send these settings to the currently active shader.
    // Envia estos ajustes al shader que esta activo actualmente.
    void Apply(Shader& shader) const;
};
