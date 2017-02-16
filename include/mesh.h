#ifndef MESH_H
#define MESH_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};


struct Mesh
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned> indices;
    unsigned materialIndex;
};

#endif // MESH_H
