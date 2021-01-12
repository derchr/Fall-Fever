#pragma once

#include <glad/glad.h>
#include "ShaderProgram.h"
#include "Texture.h"

class Framebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height, ShaderProgram* shaderProgram);
    ~Framebuffer();

    void bind();
    void unbind();

    void render();

    void changeDimensions(uint32_t width, uint32_t height);

    GLuint getTextureId()
    {
        return textures[0];
    }
    void setExposureCorrection(bool exposureCorrection);

private:
    void generateTextures(uint32_t width, uint32_t height);

    GLuint FBO;
    GLuint textures[2];

    ShaderProgram *shaderProgram;
};

enum depthMapType {DEPTHMAP_NORMAL, DEPTHMAP_CUBEMAP};

// Framebuffer without color buffer. (Shadows)
class DepthMap
{
public:
    // Normal depthMap with texture and point depthMap with cubeMap
    DepthMap(int TYPE, int RESOLUTION);

    void bind();
    void unbind();

    GLuint getFBO()
    {
        return depthMapFBO;
    }
    GLuint getDepthMap()
    {
        return depthMap;
    }
    GLuint getCubeMapId()
    {
        return cubeMap.getTextureId();
    }

private:
    GLuint depthMapFBO;

    // Either a normal depthMap is used (Directional shadows)
    // or a cubeMap is used (Point shadows)
    GLuint depthMap;
    CubeMap cubeMap;
};
