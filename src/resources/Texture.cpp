#include "Texture.h"
#include "../ShaderProgram.h"
#include "../util/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture(const TextureDescriptor &descriptor)
    : AbstractTexture(descriptor.path), m_textureType(descriptor.textureType)
{
    stbi_set_flip_vertically_on_load(1);

    int textureWidth{};
    int textureHeight{};
    int numComponents{};

    m_textureBuffer = stbi_load(resourcePath().c_str(), &textureWidth, &textureHeight, &numComponents, 0);

    m_textureWidth = static_cast<unsigned>(textureWidth);
    m_textureHeight = static_cast<unsigned>(textureHeight);
    m_numComponents = static_cast<unsigned>(numComponents);

    if (!m_textureBuffer)
        Log::logger().warn("Texture {} could not be loaded", resourcePath().string());
}

void Texture::initialize()
{
    m_initialized = true;

    GLenum internalFormat;
    GLenum dataFormat;

    switch (m_numComponents) {
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

    // Push texture to grahics card
    glGenTextures(1, &m_glId);
    glBindTexture(GL_TEXTURE_2D, m_glId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), static_cast<GLsizei>(m_textureWidth),
                 static_cast<GLsizei>(m_textureHeight), 0, dataFormat, GL_UNSIGNED_BYTE, m_textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(m_textureBuffer);
}

TextureType Texture::textureType() const
{
    return m_textureType;
}

void Texture::bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum) const
{
    std::string uniformName = "texture_";

    switch (m_textureType) {

    case TextureType::Diffuse:
        uniformName += "diffuse" + std::to_string(textureTypeNum);
        break;
    case TextureType::Specular:
        uniformName += "specular" + std::to_string(textureTypeNum);
        break;
    case TextureType::Normal:
        uniformName += "normal" + std::to_string(textureTypeNum);
        break;
    case TextureType::Height:
        uniformName += "height" + std::to_string(textureTypeNum);
        break;
    case TextureType::Gloss:
        uniformName += "gloss" + std::to_string(textureTypeNum);
        break;
    default:
        break;
    }

    // Add u_material as we store textures in a struct
    uniformName = "u_material." + uniformName;

    shaderProgram->setUniform(uniformName.c_str(), textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_glId);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
