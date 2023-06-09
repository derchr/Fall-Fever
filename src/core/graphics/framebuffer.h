#pragma once

#include "core/shader.h"

#include <entt/entt.hpp>
#include <glad/gl.h>
#include <glm/glm.hpp>

struct Framebuffer
{
    Framebuffer(glm::u32vec2 physical_dimensions);
    ~Framebuffer();

    Framebuffer(Framebuffer const&) = delete;
    auto operator=(Framebuffer const&) -> Framebuffer& = delete;

    Framebuffer(Framebuffer&& other) noexcept;
    auto operator=(Framebuffer&& other) noexcept -> Framebuffer&;

    void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer); }
    static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    void draw(Shader const& shader) const;

    GLuint color_buffer{};
    GLuint depth_stencil_buffer{};
    GLuint frame_buffer{};
};
