#pragma once

// Project include
#include "LightTypes.h"

// GLM include
#include <glm.hpp>

// Estructura para los distintos tipos de iluminacion || Lighting types structure
struct Light {
    LightType type; // Tipo || Type
    glm::vec3 position; // Posicion
    glm::vec3 direction; // Direccion (de la luz) || Lights direction
    glm::vec3 color;
    float intensity;

    bool drawSphere = false; // Mostrar objeto emisor (debug) || Show emisor object (debug flag)
};
