#include "texture.h"

#include <stdio.h>
#include <initializer_list>

#include <stb_image/stb_image.h>


Texture::Texture()
{

}

bool Texture::load(const std::string &filename, GLenum texture_format)
{
    int width, height, components;

    unsigned char *image = stbi_load(filename.c_str(), &width, &height, &components, 0);

    if (image == NULL || width == 0 || height == 0 || components == 0) {
        fprintf(stderr, "Error loading texture \'%s\'\n", filename.c_str());
        this->m_isValid = false;
        return false;
    } else {
        printf("Texture \'%s\' sucessfully loaded\n", filename.c_str());
    }

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    if (!texture_format) {
        if (components == 1) {
            texture_format = GL_RED;
        } else if (components == 2) {
            texture_format = GL_RG; // TODO: Swizzle
        } else if (components == 3) {
            texture_format = GL_RGB;
        } else {
            texture_format = GL_RGBA;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, 0, texture_format, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    this->m_handle = texture_id;
    this->m_width = width;
    this->m_height = height;
    this->m_components = components;
    this->m_isValid = true;
    return true;
}

void Texture::setSwizzle(GLenum red, GLenum green, GLenum blue, GLenum alpha)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, red);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, green);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, blue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, alpha);
}


bool Texture::isValid()
{
    return m_isValid;
}

unsigned Texture::width() const
{
    return m_width;
}

unsigned Texture::height() const
{
    return m_height;
}

GLuint Texture::handle() const
{
    return m_handle;
}

unsigned Texture::components() const
{
    return m_components;
}

Texture::~Texture()
{
    glDeleteTextures(1, &this->m_handle);
    this->m_handle = 0;
    this->m_width = 0;
    this->m_height = 0;
    this->m_components = 0;
}
