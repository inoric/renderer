#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <glm/vec3.hpp>
#include <vector>

#include "light.h"
#include "model.h"

struct SceneNode
{
    std::vector<SceneNode*> children;

    Light *light = NULL;
    Model *model = NULL;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 velocity;
    glm::vec3 angular_velocity;
};

#endif // SCENE_NODE_H
