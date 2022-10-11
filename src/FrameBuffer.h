#pragma once

#include "resources/Resource.h"

#include <glad/gl.h>

class ShaderProgram;
class CubeMap;

class AbstractFrameBuffer
{
public:
    virtual ~AbstractFrameBuffer() = 0;

    void bind() const;
    void unbind() const;
    GLuint getFBO() const;

protected:
    GLuint m_FBO;
};

class FrameBuffer : public AbstractFrameBuffer
{
public:
    FrameBuffer(uint32_t width, uint32_t height, ShaderProgram &shaderProgram);
    ~FrameBuffer();

    void drawOnEntireScreen() const;

    void changeDimensions(uint32_t width, uint32_t height);

    void setExposureCorrection(bool exposureCorrection) const;

    GLuint getTextureId() const;

private:
    void generateTextures(uint32_t width, uint32_t height);

    GLuint m_colorBuffer;
    GLuint m_depthStencilBuffer;

    ShaderProgram &m_shaderProgram;
};

class AbstractDepthMap : public AbstractFrameBuffer
{
public:
    virtual ~AbstractDepthMap() = 0;
};

// FrameBuffer without color buffer. (Shadows)
class DepthMap : public AbstractDepthMap
{
public:
    DepthMap(int resolution);
    ~DepthMap();

    GLuint getDepthMap() const;

private:
    GLuint m_depthMap;
};

class DepthMapCube : public AbstractDepthMap
{
public:
    DepthMapCube(int resolution);
    ~DepthMapCube();

    GLuint getCubeMapTextureId();

private:
    ResourceId m_cubeMap;
};
