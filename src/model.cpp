#include "model.h"

#include "util.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/texture.h>
#include <stdio.h>

Model::Model()
{

}

Mesh Model::loadMesh(const aiMesh *amesh)
{
    Mesh mesh;
    unsigned int num_vertices = amesh->mNumVertices;
    for (unsigned int vi = 0; vi < num_vertices; vi++) {
        MeshVertex vertex;
        aiVector3D pos = amesh->mVertices[vi];
        vertex.position = glm::vec3(pos.x, pos.y, pos.z);
        if (amesh->HasTextureCoords(0)) {
            aiVector3D texcoord = amesh->mTextureCoords[0][vi];
            vertex.uv = glm::vec2(texcoord.x, texcoord.y);
        }
        if (amesh->HasNormals()) {
            aiVector3D norm = amesh->mNormals[vi];
            vertex.normal = glm::vec3(norm.x, norm.y, norm.z);
        }
        if (amesh->HasTangentsAndBitangents()) {
            aiVector3D tangent = amesh->mTangents[vi];
            aiVector3D bitangent = amesh->mBitangents[vi];
            vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
            vertex.bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
        }
        mesh.vertices.push_back(vertex);
    }
    unsigned int num_faces = amesh->mNumFaces;
    for (unsigned int fi = 0; fi < num_faces; fi++) {
        aiFace face = amesh->mFaces[fi];
        for (unsigned int ii = 0; ii < face.mNumIndices; ii++) {
            unsigned int index = face.mIndices[ii];
            mesh.indices.push_back(index);
        }
    }
    mesh.materialIndex = amesh->mMaterialIndex;
    return mesh;
}

Material Model::loadMaterial(const aiMaterial *amaterial, const std::string &dirpath)
{
    Material material;
    aiString diffuseFilename, normalFilename;

    if (amaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseFilename) == AI_SUCCESS) {
        material.diffuseTexture = new Texture();
        std::string path = Util::makeRelativePath(dirpath, diffuseFilename.C_Str());
        material.diffuseTexture->load(path);
    }

    aiColor3D color(1.0f, 1.0f, 1.0f);
    amaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material.diffuseColor = glm::vec3(color.r, color.g, color.b);
    float opacity;
    amaterial->Get(AI_MATKEY_OPACITY, material.opacity);
    // printf("Color: [%f, %f, %f, %f]\n", color.r, color.g, color.b, color.a);
    // printf("Opacity: %f\n", opacity);

    if (amaterial->GetTexture(aiTextureType_NORMALS, 0, &normalFilename) == AI_SUCCESS) {
        material.normalTexture = new Texture();
        std::string path = Util::makeRelativePath(dirpath, normalFilename.C_Str());
        material.normalTexture->load(path);
    } else if (amaterial->GetTexture(aiTextureType_HEIGHT, 0, &normalFilename) == AI_SUCCESS) {
        material.normalTexture = new Texture();
        std::string path = Util::makeRelativePath(dirpath, normalFilename.C_Str());
        material.normalTexture->load(path);
    }


    return material;
}

void Model::fillBuffers()
{
    fprintf(stderr, "Filling buffer\n");
    std::vector<MeshVertex> vertices;
    std::vector<GLuint> indices;
    for (Mesh mesh : m_meshes) {
        GLuint indexOffset = vertices.size();
        vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
        for (GLuint index : mesh.indices) {
            indices.push_back(index + indexOffset); // Offset by vertices of previous meshes
        }
    }
    if (!m_vertex_buffer) {
        glGenBuffers(1, &m_vertex_buffer);
    }
    if (!m_index_buffer) {
        glGenBuffers(1, &m_index_buffer);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

const std::vector<Mesh> &Model::meshes()
{
    return m_meshes;
}

const std::vector<Material> &Model::materials()
{
    return m_materials;
}

bool Model::loadFile(const std::string &filename)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_GenNormals             |
        aiProcess_CalcTangentSpace       |
        aiProcess_GenUVCoords            |
        aiProcess_FlipUVs                |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType
    );

    if (!scene) {
        fprintf(stderr, "Unable to read model file '%s'\n", filename.c_str());
        return false;
    }

    fprintf(stderr, "Filename: %s\n", filename.c_str());
    std::string dirname = Util::dirname(filename);

    unsigned int num_materials = scene->mNumMaterials;
    for (unsigned int i = 0; i < num_materials; i++) {
        aiMaterial *material = scene->mMaterials[i];
        m_materials.push_back(loadMaterial(material, dirname));
    }

    unsigned int num_meshes = scene->mNumMeshes;
    printf("Meshes: %u\n", num_meshes);
    for (unsigned int i = 0; i < num_meshes; i++) {
        aiMesh *mesh = scene->mMeshes[i];
        m_meshes.push_back(this->loadMesh(mesh));
    }
    fillBuffers();

    return true;
}

GLuint Model::vertexBufferHandle()
{
    return m_vertex_buffer;
}

GLuint Model::indexBufferHandle()
{
    return m_index_buffer;
}
