#include "scene.h"

#include <fstream>
#include <stdio.h>
#include <json.hpp>
#include <glm/vec3.hpp>

using vec3 = glm::vec3;
using Json = nlohmann::json;

bool jsonIsVec3(Json json)
{
    return json.is_array() && json.size() == 3;
}

vec3 jsonToVec3(Json json)
{
    glm::vec3 vec;
    if (!jsonIsVec3(json)) {
        printf("Warning: Invalid vec3: %s\n", json.dump().c_str());
        return vec;
    }
    vec.x = json[0].get<float>();
    vec.y = json[1].get<float>();
    vec.z = json[2].get<float>();
    return vec;
}

SceneNode jsonToSceneNode(Json json, std::unordered_map<std::string, Model*> *models)
{
    SceneNode node;
    if (json.count("position")) {
        node.position = jsonToVec3(json["position"]);
    }
    if (json.count("rotation")) {
        node.rotation = jsonToVec3(json["rotation"]);
    }
    if (json.count("scale")) {
        if (json["scale"].is_number()) {
            float scale = json["scale"].get<float>();
            node.scale = vec3(scale, scale, scale);
        } else if (jsonIsVec3(json["scale"])) {
            node.scale = jsonToVec3(json["scale"]);
        }
    }
    if (json.count("model")) {
        std::string modelFilename = json["model"].get<std::string>();
        auto modelIt = models->find(modelFilename);
        if (modelIt == models->end()) {
            Model *newModel = new Model();
            newModel->loadFile(modelFilename);
            newModel->filename = modelFilename;
            (*models)[modelFilename] = newModel;
            node.model = newModel;
        } else {
            node.model = modelIt->second;
        }
    }
    if (json.count("children")) {
        if (!json["children"].is_array()) {
            printf("Warning: node's children not array, got this: %s\n", json["children"].dump().c_str());
        } else {
            for (Json childNodeJson : json["children"]) {
                SceneNode *child_ptr = new SceneNode(jsonToSceneNode(childNodeJson, models));
                node.children.push_back(child_ptr);
            }
        }
    }
    return node;
}

bool Scene::loadJSON(const std::string &filename)
{
    printf("Loading scene \"%s\"\n", filename.c_str());
    std::ifstream jsonIFS(filename);
    Json json = Json::parse(jsonIFS);
    printf("JSON: %s\n", json.dump(4).c_str());
    if (!json.count("rootNode")) {
        fprintf(stderr, "Root node not defined, scene loading failed\n");
        return false;
    }
    if (!json.count("camera")) {
        fprintf(stderr, "Camera position not defined, scene loading failed\n");
        return false;
    }

    if (json["camera"].count("position")) {
        this->camera.position = jsonToVec3(json["camera"]["position"]);
    }
    if (json["camera"].count("orientation")) {
        this->camera.orientation = jsonToVec3(json["camera"]["orientation"]);
    }
    this->rootNode = jsonToSceneNode(json["rootNode"], &this->models);
    return true;
}
