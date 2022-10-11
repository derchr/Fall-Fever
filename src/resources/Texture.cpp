#include "Texture.h"
#include "../ShaderProgram.h"
#include "../util/Log.h"

#include <stb/stb_image.h>

// Texture::Texture(const TextureDescriptor &descriptor) : m_textureType(descriptor.textureType)
// {
//     stbi_set_flip_vertically_on_load(1);

//     int textureWidth{};
//     int textureHeight{};
//     int numComponents{};

//     m_textureBuffer = stbi_load(resourcePath().c_str(), &textureWidth, &textureHeight, &numComponents, 0);

//     m_textureWidth = static_cast<unsigned>(textureWidth);
//     m_textureHeight = static_cast<unsigned>(textureHeight);

//     if (m_textureBuffer == nullptr) {
//         Log::logger().warn("Texture {} could not be loaded", resourcePath().string());
//     }

//     GLenum internalFormat{};
//     GLenum dataFormat{};

//     switch (numComponents) {
//     case 1:
//         internalFormat = GL_RED;
//         dataFormat = GL_RED;
//         break;
//     case 3:
//         internalFormat = (m_textureType == TextureType::Diffuse) ? GL_SRGB8 : GL_RGB8;
//         dataFormat = GL_RGB;
//         break;
//     case 4:
//         internalFormat = (m_textureType == TextureType::Diffuse) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
//         dataFormat = GL_RGBA;
//         break;
//     }

//     // Push texture to grahics card
//     glGenTextures(1, &m_glId);
//     glBindTexture(GL_TEXTURE_2D, m_glId);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2.0F);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//     glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), static_cast<GLsizei>(m_textureWidth),
//                  static_cast<GLsizei>(m_textureHeight), 0, dataFormat, GL_UNSIGNED_BYTE, m_textureBuffer);
//     glGenerateMipmap(GL_TEXTURE_2D);

//     glBindTexture(GL_TEXTURE_2D, 0);

//     stbi_image_free(m_textureBuffer);
// }

Texture::Texture(tinygltf::Texture const &texture, std::span<tinygltf::Image> images)
{
    auto sampler = texture.sampler;
    auto const &image = images[texture.source];

    m_textureType = TextureType::Diffuse;

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
