#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "opengl.h"
#include "shader.h"

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "util.h"

#include "mesh.h"
#include "model.h"
#include "material.h"
#include "texture.h"
#include "scene.h"

void glfw_error_callback(int, const char *desc) {
    fprintf(stderr, "GLFW Error: %s\n", desc);
}

GLFWwindow *createWindow() {
    GLFWwindow *window = NULL;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW.\n");
        return NULL;
    }
    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "Prozorko", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Error creating window.\n");
        glfwDestroyWindow(window);
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Vsync

    if(!gladLoadGL()) {
        fprintf(stderr, "Failed to init GLAD.\n");
        return NULL;
    }

    return window;
}

struct RenderableObject
{
    glm::mat4 matrix;
    Model *model = NULL;
    Light *light = NULL;
};

#include "mathutil.h"

std::vector<RenderableObject> getRenderablesList(const SceneNode &root, glm::mat4 *parentMatrix = nullptr)
{
    std::vector<RenderableObject> renderables;

    RenderableObject rootRenderable;
    glm::mat4 nodeMatrix = glm::translate(root.position) *
                           MathUtil::eulerRotationToMatrix(root.rotation) *
                           glm::scale(root.scale);
    if (parentMatrix) {
        nodeMatrix = (*parentMatrix) * nodeMatrix; // Make transform relative to parent
    }
    rootRenderable.matrix = nodeMatrix;
    if (root.model) {
        rootRenderable.model = root.model;
        renderables.push_back(rootRenderable);
    } else {
        rootRenderable.model = NULL;
    }
    for (auto child : root.children) {
        std::vector<RenderableObject> children = getRenderablesList(*child, &rootRenderable.matrix);
        renderables.insert(renderables.end(), children.begin(), children.end());
    }
    return renderables;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::string programName = "Renderer";
        if (argc == 1) {
            programName = std::string(argv[0]);
        }
        fprintf(stderr, "%s: scene not specified.\n", programName.c_str());
        printf("Usage: %s <scene_file_path>\n", programName.c_str());
        return 1;
    }
    std::string sceneFilePath(argv[1]);
    GLFWwindow *window = createWindow();

    Scene scene;
    scene.loadJSON(sceneFilePath);

    std::vector<RenderableObject> renderables;

    Shader shader;
    shader.addShaderFile(GL_VERTEX_SHADER, "resources/shaders/normalMapped.vert");
    shader.addShaderFile(GL_FRAGMENT_SHADER, "resources/shaders/normalMapped.fsh");
    shader.link();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    float lastTime = glfwGetTime(), currentTime = glfwGetTime();
    float delta = 0.0;
    shader.bind();
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        delta = currentTime - lastTime;

        scene.rootNode.rotation.y += delta*0.5;
        renderables = getRenderablesList(scene.rootNode);
        // printf("Frame delta: %f\n", delta);

//        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);

        glm::mat4 view = glm::lookAt(scene.camera.position, scene.camera.lookingAt(), scene.camera.upDirection());
        glm::mat4 projection = glm::perspective(45.0f, 1024.0f / 768.0f, 0.1f, 100.0f);
        for (const auto &renderable : renderables) {
            // fprintf(stderr, "Renderable has model: %s\n", renderable.model->filename.c_str());

            glm::mat4 model = renderable.matrix;
            glm::mat3 modelView3x3 = glm::mat3(view * model);
            glm::mat4 mvp = projection * view * model;

            shader.setUniform("lightPos_worldspace", glm::vec3(0.0f, 0.5f, 4.0f));
            shader.setUniform("mvp", mvp);
            shader.setUniform("model", model);
            shader.setUniform("view", view);
            shader.setUniform("modelView3x3", modelView3x3);

            glBindBuffer(GL_ARRAY_BUFFER, renderable.model->vertexBufferHandle());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.model->indexBufferHandle());

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, normal));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, uv));
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tangent));
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, bitangent));

            unsigned offset = 0;
            for (const Mesh &mesh : renderable.model->meshes()) {
                Material mat = renderable.model->materials()[mesh.materialIndex];
                shader.setUniform("opacity", mat.opacity);
                shader.setUniform("diffuseColor", mat.diffuseColor);

                glActiveTexture(GL_TEXTURE0);
                if (mat.diffuseTexture) {
                    shader.setUniform("hasDiffuseTexture", 1);
                    glBindTexture(GL_TEXTURE_2D, mat.diffuseTexture->handle());
                    shader.setUniform("diffuseSampler", 0);
                } else {
                    shader.setUniform("hasDiffuseTexture", 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }

                glActiveTexture(GL_TEXTURE1);
                if (mat.normalTexture) {
                    shader.setUniform("hasNormalTexture", 1);
                    glBindTexture(GL_TEXTURE_2D, mat.normalTexture->handle());
                    shader.setUniform("normalSampler", 1);
                } else {
                    shader.setUniform("hasNormalTexture", 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*) (offset * sizeof(GLuint)));
                offset += mesh.indices.size();
            }
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);

        glfwSwapBuffers(window);
        glfwPollEvents();
        lastTime = currentTime;
    }

    glfwDestroyWindow(window);

    return 0;
}
