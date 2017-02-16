#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include "opengl.h"
#include "mesh.h"
#include "material.h"

class Model
{
public:
    Model();
    Mesh loadMesh(const aiMesh *amesh);
    Material loadMaterial(const aiMaterial *amaterial, const std::string &dirpath);
    void fillBuffers();
    const std::vector<Mesh> &meshes();
    const std::vector<Material> &materials();
    bool loadFile(const std::string &filename);
    GLuint vertexBufferHandle();
    GLuint indexBufferHandle();

private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    GLuint m_vertex_buffer;
    GLuint m_index_buffer;
};

#endif // MODEL_H
