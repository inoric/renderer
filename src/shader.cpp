#include "shader.h"

#include <stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include "util.h"
//Result<Shader, std::string> Shader::fromFiles()
//{

//}

Shader::Shader()
{
    m_handle = glCreateProgram();
}

Shader::~Shader()
{
    for (GLuint shader : m_shaders) {
        glDetachShader(m_handle, shader);
        glDeleteShader(shader);
    }
    glDeleteProgram(m_handle);
}

bool Shader::addShaderSource(GLenum shader_type, const char *src)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        char *error_log = (char*) malloc(log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, error_log);
        fprintf(stderr, "Shader compilation error: \n%s\n", error_log);
        free(error_log);
        error_log = NULL;
        return false;
    }

    glAttachShader(m_handle, shader);
    m_shaders.push_back(shader);
    return true;
}

bool Shader::addShaderSource(GLenum shader_type, const std::string &src)
{
    return addShaderSource(shader_type, src.c_str());
}

bool Shader::addShaderFile(GLenum shader_type, const char *filename)
{
    std::string src = Util::fileToString(filename);
    if (!src.length()) {
        fprintf(stderr, "Failed to load shader file:' %s'\n", filename);
        return false;
    }
    return addShaderSource(shader_type, src);
}

bool Shader::addShaderFile(GLenum shader_type, const std::string &filename)
{
    return addShaderFile(shader_type, filename.c_str());
}

bool Shader::link()
{
    bool success;

    glLinkProgram(m_handle);
    GLint link_status = 0, link_log_length = 0;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &link_status);
    glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &link_log_length);

    char *link_log = (char*) malloc(link_log_length);
    glGetProgramInfoLog(m_handle, link_log_length, &link_log_length, link_log);
    if (link_status == GL_FALSE) {
        fprintf(stderr, "Error linking shader program pair: \n%s\n", link_log);
        success = false;
    } else {
        printf("Successfully linked program\n");
        success = true;
    }
    free(link_log);

    for (GLuint shader : m_shaders) {
        glDetachShader(m_handle, shader);
        glDeleteShader(shader);
        m_shaders.clear();
    }
    m_attrib_names = activeAttribs();
    m_uniform_names = activeUniforms();
    printf("Attributes (%lu): \n", m_attrib_names.size());
    for (auto kv : m_attrib_names) {
        printf("\t%s: %d\n", kv.first.c_str(), kv.second);
    }
    printf("Uniforms (%lu): \n", m_uniform_names.size());
    for (auto kv : m_uniform_names) {
        printf("\t%s: %d\n", kv.first.c_str(), kv.second);
    }
    return success;
}

void Shader::bind()
{
    glUseProgram(m_handle);
}

void Shader::unbind()
{
    glUseProgram(0);
}

GLint Shader::getUniformLocation(const char *uniform_name)
{
    auto it = m_uniform_names.find(uniform_name);
    if (it != m_uniform_names.end()) {
        return (*it).second;
    }
    return -1;
}

GLint Shader::getAttribLocation(const char *attrib_name)
{
    auto it = m_attrib_names.find(attrib_name);
    if (it != m_attrib_names.end()) {
        return (*it).second;
    }
    return -1;
}

void Shader::setUniform(const char *uniform_name, float value)
{
    glUniform1f(getUniformLocation(uniform_name), value);
}

void Shader::setUniform(const char *uniform_name, glm::vec2 value)
{
    glUniform2fv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::vec3 value)
{
    glUniform3fv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::vec4 value)
{
    glUniform4fv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, int value)
{
    glUniform1i(getUniformLocation(uniform_name), value);
}

void Shader::setUniform(const char *uniform_name, glm::ivec2 value)
{
    glUniform2iv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::ivec3 value)
{
    glUniform3iv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::ivec4 value)
{
    glUniform4iv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, unsigned int value)
{
    glUniform1ui(getUniformLocation(uniform_name), value);
}

void Shader::setUniform(const char *uniform_name, glm::uvec2 value)
{
    glUniform2uiv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::uvec3 value)
{
    glUniform3uiv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::uvec4 value)
{
    glUniform4uiv(getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::mat2 value)
{
    glUniformMatrix2fv(getUniformLocation(uniform_name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::mat3 value)
{
    glUniformMatrix3fv(getUniformLocation(uniform_name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform(const char *uniform_name, glm::mat4 value)
{
    glUniformMatrix4fv(getUniformLocation(uniform_name), 1, GL_FALSE, glm::value_ptr(value));
}

std::map<std::string, GLuint> Shader::activeUniforms()
{
    std::map<std::string, GLuint> uniforms;
    GLint num_uniforms = 0;
    glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, &num_uniforms);
    GLint max_uniform_name_length = 0;
    glGetProgramiv(m_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);
    std::vector<GLchar> name_vec(max_uniform_name_length);
    for (int i = 0; i < num_uniforms; i++) {
        GLint arr_size = 0;
        GLenum type = 0;
        GLsizei name_length = 0;
        glGetActiveUniform(m_handle, i, name_vec.size(), &name_length, &arr_size, &type, &name_vec[0]);
        std::string name(name_vec.data(), name_length);
        uniforms[name] = glGetUniformLocation(m_handle, name.c_str());
    }
    return uniforms;
}

std::map<std::string, GLuint> Shader::activeAttribs()
{
    std::map<std::string, GLuint> attribs;
    GLint num_attribs = 0;
    glGetProgramiv(m_handle, GL_ACTIVE_ATTRIBUTES, &num_attribs);
    GLint max_attrib_name_length = 0;
    glGetProgramiv(m_handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_name_length);
    std::vector<GLchar> name_vec(max_attrib_name_length);
    for (int i = 0; i < num_attribs; i++) {
        GLint arr_size = 0;
        GLenum type = 0;
        GLsizei name_length = 0;
        glGetActiveAttrib(m_handle, i, name_vec.size(), &name_length , &arr_size, &type, &name_vec[0]);
        std::string name(name_vec.data(), name_length);
        attribs[name] = glGetAttribLocation(m_handle, name.c_str());
    }
    return attribs;
}
