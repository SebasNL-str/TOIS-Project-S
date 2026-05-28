#pragma once
#pragma once
#include <glm.hpp>
#include "LightTypes.h"

struct Light
{
    LightType type;

    glm::vec3 position;   // usado en Point/Spot | for point/spot light type
    glm::vec3 direction;  // usado en Directional/Spot | for directiona/spot light type

    glm::vec3 color;
    float intensity;
};