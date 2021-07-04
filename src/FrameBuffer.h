#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include <glad/glad.h>

class FrameBuffer
{
public:
    FrameBuffer(uint32_t width, uint32_t height, ShaderProgram *shaderProgram);
    ~FrameBuffer();

    void bind();
    void unbind();

    void render();

    void changeDimensions(uint32_t width, uint32_t height);

    GLuint getTextureId();
    void setExposureCorrection(bool exposureCorrection);

private:
    void generateTextures(uint32_t width, uint32_t height);

    GLuint m_FBO;
    GLuint m_colorBuffer;
    GLuint m_depthStencilBuffer;

    ShaderProgram *m_shaderProgram;
};

enum class DepthMapType
{
    Normal,
    CubeMap
};

// FrameBuffer without color buffer. (Shadows)
class DepthMap
{
public:
    // Normal depthMap with texture and point depthMap with cubeMap
    DepthMap(DepthMapType type, int RESOLUTION);
    ~DepthMap();

    void bind();
    void unbind();

    GLuint getFBO();
    GLuint getDepthMap();
    GLuint getCubeMapId();

private:
    GLuint m_depthMapFBO;

    // Either a normal depthMap is used (Directional shadows)
    // or a cubeMap is used (Point shadows)
    GLuint m_depthMap;
    CubeMap *m_cubeMap;
};
