#include "Texture.h"
#include "../ShaderProgram.h"
#include "../util/Log.h"

Texture::Texture(tinygltf::Texture const &texture, std::span<tinygltf::Image> images, TextureType textureType)
    : m_textureType(textureType)
{
    auto sampler = texture.sampler;
    auto const &image = images[texture.source];

    uint32_t width = image.width;
    uint32_t height = image.height;
    unsigned int components = image.component;

    GLenum internalFormat{};
    GLenum dataFormat{};

    switch (components) {
    case 1:
        internalFormat = GL_RED;
        dataFormat = GL_RED;
        break;
    case 3:
        internalFormat = (m_textureType == TextureType::Diffuse) ? GL_SRGB8 : GL_RGB8;
        dataFormat = GL_RGB;
        break;
    case 4:
        internalFormat = (m_textureType == TextureType::Diffuse) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    }

    glGenTextures(1, &m_glId);
    glBindTexture(GL_TEXTURE_2D, m_glId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2.0F);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), 0, dataFormat, GL_UNSIGNED_BYTE, image.image.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    Log::logger().trace(R"(Loaded texture "{}".)", image.name);
}

auto Texture::textureType() const -> TextureType
{
    return m_textureType;
}

void Texture::bind(uint8_t textureUnit, ShaderProgram const &shaderProgram) const
{
    std::string uniformName = "texture_";

    switch (m_textureType) {
    case TextureType::Diffuse:
        uniformName += "diffuse";
        break;
    case TextureType::Normal:
        uniformName += "normal";
        break;
    default:
        break;
    }

    // Add u_material as we store textures in a struct
    uniformName = "u_material." + uniformName;

    shaderProgram.setUniform(uniformName, textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_glId);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
