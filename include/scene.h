#ifndef SCENE_H
#define SCENE_H

#include "scenenode.h"
#include "camera.h"

struct Scene
{
    SceneNode rootNode;
    Camera camera;
};

#endif // SCENE_H
