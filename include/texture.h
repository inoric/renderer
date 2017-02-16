#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl.h"

class Texture
{
public:
    Texture();
    ~Texture();

    bool load(const std::string &filename, GLenum texture_format = 0);
    void setSwizzle(GLenum red, GLenum green, GLenum blue, GLenum alpha);
    bool isValid();
    unsigned width() const;
    unsigned height() const;
    GLuint handle() const;
    unsigned components() const;

private:
    bool m_isValid;
    unsigned m_width;
    unsigned m_height;
    unsigned m_components;
    GLuint m_handle;
};

#endif // TEXTURE_H
