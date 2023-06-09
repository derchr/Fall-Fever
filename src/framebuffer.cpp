#include "framebuffer.h"
#include "util/Log.h"

#include <array>

Framebuffer::Framebuffer(glm::u32vec2 physical_dimensions)
{
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    auto width = static_cast<GLsizei>(physical_dimensions.x);
    auto height = static_cast<GLsizei>(physical_dimensions.y);

    // Create new textures
    glGenTextures(1, &color_buffer);
    glGenRenderbuffers(1, &depth_stencil_buffer);

    {
        glBindTexture(GL_TEXTURE_2D, color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    std::array<unsigned, 3> attachments = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments.data());

    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log::logger().error("Framebuffer not complete");
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : color_buffer(other.color_buffer),
      depth_stencil_buffer(other.depth_stencil_buffer),
      frame_buffer(other.frame_buffer)
{
    other.color_buffer = 0;
    other.depth_stencil_buffer = 0;
    other.frame_buffer = 0;
}

auto Framebuffer::operator=(Framebuffer&& other) noexcept -> Framebuffer&
{
    glDeleteFramebuffers(1, &frame_buffer);
    glDeleteTextures(1, &color_buffer);
    glDeleteRenderbuffers(1, &depth_stencil_buffer);

    color_buffer = other.color_buffer;
    depth_stencil_buffer = other.depth_stencil_buffer;
    frame_buffer = other.frame_buffer;

    other.color_buffer = 0;
    other.depth_stencil_buffer = 0;
    other.frame_buffer = 0;

    return *this;
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &frame_buffer);
    glDeleteTextures(1, &color_buffer);
    glDeleteRenderbuffers(1, &depth_stencil_buffer);
}

void Framebuffer::draw(Shader const& shader) const
{
    GLint polygon_mode{};
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_buffer);

    shader.set_uniform("u_texture", 0);

    // A VAO is necessary although no data is stored in it
    GLuint vao{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(polygon_mode));
    Shader::unbind();
}
