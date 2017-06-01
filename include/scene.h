#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <unordered_map>
#include "scenenode.h"
#include "camera.h"

struct Scene
{
    SceneNode rootNode;
    Camera camera;
    std::unordered_map<std::string, Model*> models;

    bool loadJSON(const std::string &filename);
};

#endif // SCENE_H
