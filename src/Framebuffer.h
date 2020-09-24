#pragma once

#include <glad/glad.h>
#include "ShaderProgram.h"

class Framebuffer {

public:

    Framebuffer(uint32_t width, uint32_t height, ShaderProgram *ShaderProgram);
    ~Framebuffer();

    void bind();
    void unbind();

    void render();

    GLuint getTextureId() {
        return textures[0];
    }

private:

    GLuint FBO;
    GLuint textures[2];

    ShaderProgram *shaderProgram;

};


class DepthMap {

public:

    DepthMap(int RESOLUTION);

    void bind();
    void unbind();

    GLuint getDepthMap() { return depthMap; }

private:

    GLuint depthMapFBO;
    GLuint depthMap;

};