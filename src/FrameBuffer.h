#pragma once

#include <glad/gl.h>

class ShaderProgram;

class Framebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height, ShaderProgram &shaderProgram);
    ~Framebuffer();

    void bind() const;
    static void unbind();
    GLuint getFBO() const;

    void drawOnEntireScreen() const;

    void changeDimensions(uint32_t width, uint32_t height);

    void setExposureCorrection(bool exposureCorrection) const;

    GLuint getTextureId() const;

private:
    void generateTextures(uint32_t width, uint32_t height);

    GLuint m_colorBuffer;
    GLuint m_depthStencilBuffer;
    GLuint m_FBO;

    ShaderProgram &m_shaderProgram;
};
