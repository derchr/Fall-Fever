#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

class Shader;

class Framebuffer
{
public:
    Framebuffer(glm::u32vec2 physical_dimensions, Shader &shader);
    ~Framebuffer();

    void bind() const;
    static void unbind();
    GLuint getFBO() const;

    void drawOnEntireScreen() const;

    void updateDimensions(glm::u32vec2 physical_dimensions);
    void setExposureCorrection(bool exposureCorrection) const;

    GLuint getTextureId() const;

private:
    void generateTextures(uint32_t width, uint32_t height);

    GLuint m_colorBuffer;
    GLuint m_depthStencilBuffer;
    GLuint m_FBO;

    Shader &m_shader;
};
