#pragma once

#include "TextureType.h"

#include <glad/gl.h>
#include <span>
#include <string>
#include <tiny_gltf.h>

class ShaderProgram;

// struct TextureDescriptor
// {
//     std::string path;
//     TextureType textureType;
// };

class Texture
{
public:
    // Texture(const TextureDescriptor &descriptor);
    Texture(tinygltf::Texture const &texture, std::span<tinygltf::Image> images);

    [[nodiscard]] auto textureType() const -> TextureType;

    void bind(uint8_t textureUnit, ShaderProgram const &shaderProgram) const;
    static void unbind();

private:
    TextureType m_textureType;
    GLuint m_glId = 0;
};