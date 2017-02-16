#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"

struct Material
{
    Texture *diffuseTexture;
    Texture *normalTexture;
};

#endif // MATERIAL_H
