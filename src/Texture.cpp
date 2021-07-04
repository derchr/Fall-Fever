#include "Texture.h"

#include <iostream>
#include <stb/stb_image.h>

Texture::Texture(const std::string &texturePath, TextureType textureType)
    : m_texturePath(texturePath), m_textureType(textureType)
{
    stbi_set_flip_vertically_on_load(1);
    auto *textureBuffer = stbi_load(texturePath.c_str(), &m_textureWidth, &m_textureHeight, &m_numComponents, 0);

    GLenum internalFormat;
    GLenum dataFormat;
    if (m_numComponents == 1) {
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    } else if (m_numComponents == 3) {
        internalFormat = (textureType == TextureType::Diffuse) ? GL_SRGB8 : GL_RGB8;
        dataFormat = GL_RGB;
    } else if (m_numComponents == 4) {
        internalFormat = (textureType == TextureType::Diffuse) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    // Push texture to grahics card
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (!textureBuffer) {
        std::cout << "[Warning] Texture " << texturePath << " not found!" << std::endl;
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_textureWidth, m_textureHeight, 0, dataFormat, GL_UNSIGNED_BYTE,
                 textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(textureBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureId);
}

void Texture::bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum)
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
    }

    // Add u_material as we store textures in a struct
    uniformName = "u_material." + uniformName;

    shaderProgram->setUniform(uniformName.c_str(), textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureType Texture::getTextureType()
{
    return m_textureType;
}

std::string Texture::getPath()
{
    return m_texturePath;
}

GLuint Texture::getTextureId()
{
    return m_textureId;
}

CubeMap::CubeMap(const char *texturePseudoPath)
{
    // Reserve space in vector so that elements can be accessed explicitly.
    m_texturePaths.resize(CUBEMAP_FACES_NUM_ITEMS);
    fillTexturePathVector(texturePseudoPath);

    stbi_set_flip_vertically_on_load(0);

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < CUBEMAP_FACES_NUM_ITEMS; i++) {

        int32_t numComponents;
        auto *textureBuffer =
            stbi_load(m_texturePaths[i].c_str(), &m_textureWidth, &m_textureHeight, &numComponents, 0);

        GLenum internalFormat;
        GLenum dataFormat;
        if (numComponents == 1) {
            internalFormat = GL_RED;
            dataFormat = GL_RED;
        } else if (numComponents == 3) {
            internalFormat = GL_SRGB8;
            dataFormat = GL_RGB;
        } else if (numComponents == 4) {
            internalFormat = GL_SRGB8_ALPHA8;
            dataFormat = GL_RGBA;
        }

        if (!textureBuffer) {
            std::cout << "[Warning] CubeMap Texture " << m_texturePaths[i].c_str() << " not found!" << std::endl;
            return;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, m_textureWidth, m_textureHeight, 0,
                     dataFormat, GL_UNSIGNED_BYTE, textureBuffer);

        stbi_image_free(textureBuffer);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::CubeMap(int RESOLUTION) : m_textureWidth(RESOLUTION), m_textureHeight(RESOLUTION)
{

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    const unsigned int CUBEMAP_NUM_FACES = 6;
    for (unsigned int i = 0; i < CUBEMAP_NUM_FACES; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, RESOLUTION, RESOLUTION, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1, &m_textureId);
}

void CubeMap::bind(ShaderProgram *shaderProgram)
{
    std::string uniformName = "u_skybox";

    shaderProgram->setUniform(uniformName.c_str(), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);
}

void CubeMap::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::fillTexturePathVector(const char *texturePseudoPath)
{
    for (unsigned int i = 0; i < CUBEMAP_FACES_NUM_ITEMS; i++) {
        m_texturePaths[cm_front] = std::string(texturePseudoPath) + "front.png";
        m_texturePaths[cm_back] = std::string(texturePseudoPath) + "back.png";
        m_texturePaths[cm_top] = std::string(texturePseudoPath) + "top.png";
        m_texturePaths[cm_bottom] = std::string(texturePseudoPath) + "bottom.png";
        m_texturePaths[cm_left] = std::string(texturePseudoPath) + "left.png";
        m_texturePaths[cm_right] = std::string(texturePseudoPath) + "right.png";
    }
}

GLuint CubeMap::getTextureId()
{
    return m_textureId;
}
