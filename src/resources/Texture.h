#pragma once

#include "TextureType.h"

#include <glad/gl.h>
#include <span>
#include <string>
#include <tiny_gltf.h>

class ShaderProgram;

class Texture
{
public:
    Texture(tinygltf::Texture const &texture, std::span<tinygltf::Image> images, TextureType textureType);

    [[nodiscard]] auto textureType() const -> TextureType;

    void bind(uint8_t textureUnit, ShaderProgram const &shaderProgram) const;
    static void unbind();

private:
    TextureType m_textureType;
    GLuint m_glId = 0;
};