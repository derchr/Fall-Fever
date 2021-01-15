#include "Texture.h"

#include <stb/stb_image.h>
#include <iostream>

Texture::Texture(const char *texturePath, uint8_t textureType)
{
    this->texturePath = texturePath;
    this->textureType = textureType;

    stbi_set_flip_vertically_on_load(1);
    auto *textureBuffer = stbi_load(texturePath, &textureWidth, &textureHeight, &numComponents, 0);

    GLenum internalFormat;
    GLenum dataFormat;
    if (numComponents == 1) {
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    } else if (numComponents == 3) {
        internalFormat = (textureType == texture_diffuse) ? GL_SRGB8 : GL_RGB8;
        dataFormat = GL_RGB;
    } else if (numComponents == 4) {
        internalFormat = (textureType == texture_diffuse) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    // Push texture to grahics card
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    if (!textureBuffer) {
        std::cout << "[Warning] Texture " << texturePath << " not found!" << std::endl;
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureWidth, textureHeight, 0, dataFormat, GL_UNSIGNED_BYTE, textureBuffer);
    //glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(textureBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureId);
}

void Texture::bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum)
{
    std::string uniformName = "texture_";

    switch (textureType) {

    case texture_diffuse:
        uniformName += "diffuse" + std::to_string(textureTypeNum);
        break;
    case texture_specular:
        uniformName += "specular" + std::to_string(textureTypeNum);
        break;
    case texture_normal:
        uniformName += "normal" + std::to_string(textureTypeNum);
        break;
    case texture_height:
        uniformName += "height" + std::to_string(textureTypeNum);
        break;
    case texture_gloss:
        uniformName += "gloss" + std::to_string(textureTypeNum);
        break;
    }

    // Add u_material as we store textures in a struct
    uniformName = "u_material." + uniformName;

    shaderProgram->setUniform(uniformName.c_str(), textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

uint8_t Texture::getTextureType()
{
    return textureType;
}

std::string Texture::getPath()
{
    return texturePath;
}

GLuint Texture::getTextureId()
{
    return textureId;
}

CubeMap::CubeMap(const char *texturePseudoPath)
{
    // Reserve space in vector so that elements can be accessed explicitly.
    texturePaths.resize(CUBEMAP_FACES_NUM_ITEMS);
    fillTexturePathVector(texturePseudoPath);

    stbi_set_flip_vertically_on_load(0);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < CUBEMAP_FACES_NUM_ITEMS; i++) {

        int32_t numComponents;
        auto *textureBuffer = stbi_load(texturePaths[i].c_str(), &textureWidth, &textureHeight, &numComponents, 0);

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
            std::cout << "[Warning] CubeMap Texture " << texturePaths[i].c_str() << " not found!" << std::endl;
            return;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, textureWidth, textureHeight, 0, dataFormat, GL_UNSIGNED_BYTE, textureBuffer);

        stbi_image_free(textureBuffer);

    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::CubeMap(int RESOLUTION) :
    textureWidth(RESOLUTION),
    textureHeight(RESOLUTION)
{

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    const unsigned int CUBEMAP_NUM_FACES = 6;
    for (unsigned int i = 0; i < CUBEMAP_NUM_FACES; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, RESOLUTION, RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1, &textureId);
}

void CubeMap::bind(ShaderProgram *shaderProgram)
{
    std::string uniformName = "u_skybox";

    shaderProgram->setUniform(uniformName.c_str(), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
}

void CubeMap::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::fillTexturePathVector(const char *texturePseudoPath)
{
    for (unsigned int i = 0; i < CUBEMAP_FACES_NUM_ITEMS; i++) {
        texturePaths[cm_front]  = std::string(texturePseudoPath) + "front.png";
        texturePaths[cm_back]   = std::string(texturePseudoPath) + "back.png";
        texturePaths[cm_top]    = std::string(texturePseudoPath) + "top.png";
        texturePaths[cm_bottom] = std::string(texturePseudoPath) + "bottom.png";
        texturePaths[cm_left]   = std::string(texturePseudoPath) + "left.png";
        texturePaths[cm_right]  = std::string(texturePseudoPath) + "right.png";
    }
}

GLuint CubeMap::getTextureId()
{
    return textureId;
}
