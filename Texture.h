#pragma once

#include "ShaderProgram.h"

#include <cstdint>
#include <glad/glad.h>
#include <string>

enum textureType{texture_diffuse, texture_specular, texture_normal, texture_height, TEXTURE_TYPE_NUM_ITEMS};

class Texture {

public:

    Texture(const char* texturePath, uint8_t textureType);
    ~Texture();

    void bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum);
    void unbind();

    uint8_t getTextureType() { return textureType; }

private:

    std::string texturePath;

    int32_t textureWidth;
    int32_t textureHeight;
    int32_t bitsPerPixel;

    GLuint textureId;

    uint8_t textureType;

};