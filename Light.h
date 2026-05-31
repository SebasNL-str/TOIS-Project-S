#pragma once
#pragma once
#include <glm.hpp>
#include "LightTypes.h"

struct Light {
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    bool drawSphere = false; // nuevo flag
};
