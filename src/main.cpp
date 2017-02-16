#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "opengl.h"
#include "shader.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "util.h"

#include "mesh.h"
#include "model.h"
#include "material.h"
#include "texture.h"

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

    window = glfwCreateWindow(1024, 768, "Hello, world", NULL, NULL);
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

int main()
{
    GLFWwindow *window = createWindow();

    Shader shader;
    shader.addShaderFile(GL_VERTEX_SHADER, "resources/test.vert");
    shader.addShaderFile(GL_FRAGMENT_SHADER, "resources/test.fsh");
    shader.link();

    Texture tex;
    tex.load("resources/brick_wall2-1024/brick_wall2-diff-1024.tga");

    Texture texNormal;
    texNormal.load("resources/brick_wall2-1024/brick_wall2-nor-1024.tga");

    Texture texSpecular;
    texSpecular.load("resources/brick_wall2-1024/brick_wall2-spec-1024.tga");
    texSpecular.setSwizzle(GL_RED, GL_RED, GL_RED, GL_ONE);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    Model thing;
    thing.loadFile("resources/wood_crates/crate1.obj");

    float rot = 0.0f;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    float lastTime = glfwGetTime(), currentTime = glfwGetTime();
    float delta = 0.0;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        delta = currentTime - lastTime;
    //    printf("Frame delta: %f\n", delta);

//        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
        rot += delta * 25;
        glm::mat4 view = glm::lookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0.5, -1), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(45.0f, 1024.0f / 768.0f, 0.1f, 100.0f);
        glm::mat4 model =
            glm::translate(glm::vec3(0.0f, -0.5f, -2.0f)) *
            glm::rotate(glm::radians(rot), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::rotate(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::scale(4.0f * glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat3 modelView3x3 = glm::mat3(view * model);
        glm::mat4 mvp = projection * view * model;

        shader.setUniform("lightPos_worldspace", glm::vec3(0.0f, 0.5f, 3.0f));
        shader.setUniform("mvp", mvp);
        shader.setUniform("model", model);
        shader.setUniform("view", view);
        shader.setUniform("modelView3x3", modelView3x3);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texNormal.handle());
        shader.setUniform("normalSampler", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texSpecular.handle());
        shader.setUniform("specularSampler", 2);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, thing.vertexBufferHandle());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thing.indexBufferHandle());

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, uv));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tangent));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, bitangent));

        unsigned offset = 0;
        for (auto mesh : thing.meshes()) {
            Material mat = thing.materials()[mesh.materialIndex];
            if (mat.diffuseTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat.diffuseTexture->handle());
                shader.setUniform("diffuseSampler", 0);
            }
            if (mat.normalTexture) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mat.normalTexture->handle());
                shader.setUniform("normalSampler", 1);
            }

            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*) (offset * sizeof(GLuint)));
            offset += mesh.indices.size();
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);

        shader.unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
        lastTime = currentTime;
    }

    glfwDestroyWindow(window);

    return 0;
}
