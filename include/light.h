#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>

struct Light
{
    glm::vec3 color;
    float intensity;
};

#endif // LIGHT_H
