#pragma once

#include <glad/glad.h>

class Framebuffer {

public:

    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    void bind();
    void unbind();

    GLuint getTextureId() {
        return textures[0];
    }

private:

    GLuint FBO;
    GLuint textures[2];

};