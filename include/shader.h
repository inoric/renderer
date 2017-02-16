#ifndef SHADER_H
#define SHADER_H

#include "opengl.h"
#include <vector>
#include <map>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader();
    ~Shader();

    bool addShaderSource(GLenum shader_type, const char *src);
    bool addShaderSource(GLenum shader_type, const std::string &src);
    bool addShaderFile(GLenum shader_type, const char *filename);
    bool addShaderFile(GLenum shader_type, const std::string &filename);

    bool link();
    void bind();
    void unbind();

    GLint getUniformLocation(const char *uniform_name);
    GLint getAttribLocation(const char *attrib_name);
    std::map<std::string, GLuint> activeUniforms();
    std::map<std::string, GLuint> activeAttribs();

    void setUniform(const char *uniform_name, float value);
    void setUniform(const char *uniform_name, glm::vec2 value);
    void setUniform(const char *uniform_name, glm::vec3 value);
    void setUniform(const char *uniform_name, glm::vec4 value);
    void setUniform(const char *uniform_name, int value);
    void setUniform(const char *uniform_name, glm::ivec2 value);
    void setUniform(const char *uniform_name, glm::ivec3 value);
    void setUniform(const char *uniform_name, glm::ivec4 value);
    void setUniform(const char *uniform_name, unsigned int value);
    void setUniform(const char *uniform_name, glm::uvec2 value);
    void setUniform(const char *uniform_name, glm::uvec3 value);
    void setUniform(const char *uniform_name, glm::uvec4 value);
    void setUniform(const char *uniform_name, glm::mat2 value);
    void setUniform(const char *uniform_name, glm::mat3 value);
    void setUniform(const char *uniform_name, glm::mat4 value);

private:
    GLuint m_handle;
    std::vector<GLuint> m_shaders;
    std::map<std::string, GLuint> m_uniform_names;
    std::map<std::string, GLuint> m_attrib_names;
};

#endif // SHADER_H
