#pragma once

#include "glm/glm.hpp"

struct Particle
{
    glm::vec3 Pos;
    glm::vec3 Vel;
    glm::vec3 Acc;
    glm::vec3 Color;
    float Density;
    float Pressure;
    float Radius;
    unsigned int HashVal;
};