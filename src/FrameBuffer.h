#pragma once

#include <glad/gl.h>

class Shader;

class Framebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height, Shader &shader);
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

    Shader &m_shader;
};
