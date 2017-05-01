#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include <glm/vec3.hpp>

struct Material
{
    glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    Texture *diffuseTexture = NULL;
    float opacity = 1.0f;

    Texture *normalTexture = NULL;

    Texture *specularTexture = NULL;
    float specularIntensity = 1.0f;
};

#endif // MATERIAL_H
