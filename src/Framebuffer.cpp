#include "Framebuffer.h"

#include <cstddef>
#include <iostream>

Framebuffer::Framebuffer(uint32_t width, uint32_t height, ShaderProgram *shaderProgram) :
    shaderProgram(shaderProgram)
{
    glGenFramebuffers(1, &FBO);

    generateTextures(width, height);
    setExposureCorrection(true);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &colorBuffer);
    glDeleteRenderbuffers(1, &depthStencilBuffer);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::render()
{
    // Disable wireframe mode
    GLint wireframe;
    glGetIntegerv(GL_POLYGON_MODE, &wireframe);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shaderProgram->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, getTextureId());

    GLint location = glGetUniformLocation(shaderProgram->getShaderProgramId(), "u_texture");
    glUniform1i(location, 0);

    // A VAO is necessary although no data is stored in it
    GLuint temp_vao;
    glGenVertexArrays(1, &temp_vao);
    glBindVertexArray(temp_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe);
    shaderProgram->unbind();
}

void Framebuffer::changeDimensions(uint32_t width, uint32_t height)
{
    // Delete old textures
    glDeleteTextures(1, &colorBuffer);
    glDeleteRenderbuffers(1, &depthStencilBuffer);

    generateTextures(width, height);
}

void Framebuffer::generateTextures(uint32_t width, uint32_t height)
{
    bind();

    // Create new textures
    glGenTextures(1, &colorBuffer);
    glGenRenderbuffers(1, &depthStencilBuffer);

    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "[Error] Framebuffer is not complete!" << std::endl;

    unbind();
}


void Framebuffer::setExposureCorrection(bool exposureCorrection)
{
    shaderProgram->bind();
    shaderProgram->setUniform("u_exposureCorrection", exposureCorrection);
    shaderProgram->unbind();
}

GLuint Framebuffer::getTextureId()
{
    return colorBuffer;
}

DepthMap::DepthMap(int TYPE, int RESOLUTION)
{
    glGenFramebuffers(1, &depthMapFBO);
    bind();

    if (TYPE == DEPTHMAP_NORMAL) {

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, RESOLUTION, RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

    } else if (TYPE == DEPTHMAP_CUBEMAP) {
        cubeMap = new CubeMap(RESOLUTION);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMap->getTextureId(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "[Error] Framebuffer is not complete!" << std::endl;

    unbind();
}

DepthMap::~DepthMap()
{
    if(cubeMap)
        delete cubeMap;
}


void DepthMap::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
}

void DepthMap::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint DepthMap::getFBO()
{
    return depthMapFBO;
}

GLuint DepthMap::getDepthMap()
{
    return depthMap;
}

GLuint DepthMap::getCubeMapId()
{
    return cubeMap->getTextureId();
}
