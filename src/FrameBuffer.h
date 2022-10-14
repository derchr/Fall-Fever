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

class Framebuffer : public AbstractFrameBuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height, ShaderProgram &shaderProgram);
    ~Framebuffer();

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
