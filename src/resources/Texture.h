#pragma once

#include "TextureType.h"

#include <filesystem>
#include <fx/gltf.h>
#include <glad/gl.h>
#include <span>
#include <string>

class ShaderProgram;

class Texture
{
public:
    Texture(fx::gltf::Texture const &texture,
            std::filesystem::path const &working_directory,
            std::span<fx::gltf::Image> images,
            std::span<fx::gltf::BufferView> bufferViews,
            std::span<fx::gltf::Buffer> buffers,
            std::span<fx::gltf::Sampler> samplers,
            TextureType textureType);

    [[nodiscard]] auto textureType() const -> TextureType;

    void bind(uint8_t textureUnit, ShaderProgram const &shaderProgram) const;
    static void unbind();

private:
    TextureType m_textureType;
    GLuint m_glId = 0;
};