#include "FrameBuffer.h"
#include "ShaderProgram.h"
#include "resources/CubeMap.h"
#include "resources/ResourceHandler.h"
#include "resources/Texture.h"
#include "util/Log.h"

#include <cstddef>

AbstractFrameBuffer::~AbstractFrameBuffer()
{
}

void AbstractFrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void AbstractFrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint AbstractFrameBuffer::getFBO() const
{
    return m_FBO;
}

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, ShaderProgram &shaderProgram) : m_shaderProgram(shaderProgram)
{
    glGenFramebuffers(1, &m_FBO);

    generateTextures(width, height);
    setExposureCorrection(true);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_colorBuffer);
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);
}

GLuint FrameBuffer::getTextureId() const
{
    return m_colorBuffer;
}

void FrameBuffer::drawOnEntireScreen() const
{
    // Disable wireframe mode
    GLint wireframe;
    glGetIntegerv(GL_POLYGON_MODE, &wireframe);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_shaderProgram.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, getTextureId());

    GLint location = glGetUniformLocation(m_shaderProgram.getShaderProgramId(), "u_texture");
    glUniform1i(location, 0);

    // A VAO is necessary although no data is stored in it
    GLuint temp_vao;
    glGenVertexArrays(1, &temp_vao);
    glBindVertexArray(temp_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(wireframe));
    m_shaderProgram.unbind();
}

void FrameBuffer::changeDimensions(uint32_t width, uint32_t height)
{
    // Delete old textures
    glDeleteTextures(1, &m_colorBuffer);
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);

    generateTextures(width, height);
}

void FrameBuffer::generateTextures(uint32_t width, uint32_t height)
{
    bind();

    // Create new textures
    glGenTextures(1, &m_colorBuffer);
    glGenRenderbuffers(1, &m_depthStencilBuffer);

    glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::logger().error("Framebuffer not complete");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    unbind();
}

void FrameBuffer::setExposureCorrection(bool exposureCorrection) const
{
    m_shaderProgram.bind();
    m_shaderProgram.setUniform("u_exposureCorrection", exposureCorrection);
    m_shaderProgram.unbind();
}

AbstractDepthMap::~AbstractDepthMap()
{
}

DepthMap::DepthMap(int RESOLUTION)
{
    glGenFramebuffers(1, &m_FBO);
    bind();

    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, RESOLUTION, RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::logger().error("Framebuffer not complete");

    unbind();
}

DepthMap::~DepthMap()
{
    // if (m_cubeMap)
    //    delete m_cubeMap;
}

GLuint DepthMap::getDepthMap() const
{
    return m_depthMap;
}

DepthMapCube::DepthMapCube(int resolution)
{
    glGenFramebuffers(1, &m_FBO);
    bind();

    m_cubeMap = ResourceHandler::instance().registerResource<InternalCubeMap>(resolution);
    GLuint glId = std::static_pointer_cast<InternalCubeMap>(ResourceHandler::instance().resource(m_cubeMap))->glId();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, glId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::logger().error("Framebuffer not complete");

    unbind();
}

DepthMapCube::~DepthMapCube()
{
}

GLuint DepthMapCube::getCubeMapTextureId()
{
    return std::static_pointer_cast<InternalCubeMap>(ResourceHandler::instance().resource(m_cubeMap))->glId();
}
