#include "FrameBuffer.h"
#include "shader.h"
#include "util/Log.h"

#include <cstddef>
#include <glm/fwd.hpp>

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::getFBO() const
{
    return m_FBO;
}

Framebuffer::Framebuffer(glm::u32vec2 physical_dimensions, Shader &shader) : m_shader(shader)
{
    glGenFramebuffers(1, &m_FBO);

    generateTextures(physical_dimensions.x, physical_dimensions.y);
    setExposureCorrection(true);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_colorBuffer);
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);
}

GLuint Framebuffer::getTextureId() const
{
    return m_colorBuffer;
}

void Framebuffer::drawOnEntireScreen() const
{
    // Disable wireframe mode
    GLint wireframe;
    glGetIntegerv(GL_POLYGON_MODE, &wireframe);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_shader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, getTextureId());

    m_shader.set_uniform("u_texture", 0);

    // A VAO is necessary although no data is stored in it
    GLuint temp_vao;
    glGenVertexArrays(1, &temp_vao);
    glBindVertexArray(temp_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(wireframe));
    m_shader.unbind();
}

void Framebuffer::updateDimensions(glm::u32vec2 physical_dimensions)
{
    // Delete old textures
    glDeleteTextures(1, &m_colorBuffer);
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);

    generateTextures(physical_dimensions.x, physical_dimensions.y);
}

void Framebuffer::generateTextures(uint32_t width, uint32_t height)
{
    bind();

    // Create new textures
    glGenTextures(1, &m_colorBuffer);
    glGenRenderbuffers(1, &m_depthStencilBuffer);

    {
        glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0,
                     GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    std::array<unsigned, 3> attachments = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments.data());

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::logger().error("Framebuffer not complete");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    unbind();
}

void Framebuffer::setExposureCorrection(bool exposureCorrection) const
{
    m_shader.bind();
    m_shader.set_uniform("u_exposureCorrection", exposureCorrection);
    m_shader.unbind();
}
