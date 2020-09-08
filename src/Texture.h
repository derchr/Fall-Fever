#pragma once

#include "ShaderProgram.h"

#include "defines.h"
#include <cstdint>
#include <glad/glad.h>
#include <string>

class Texture {

public:

    Texture(const char* texturePath, uint8_t textureType);
    ~Texture();

    void bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum);
    void unbind();

    uint8_t getTextureType() { return textureType; }
    std::string getPath() { return texturePath; }

private:

    std::string texturePath;

    int32_t textureWidth;
    int32_t textureHeight;
    int32_t bitsPerPixel;

    GLuint textureId;

    uint8_t textureType;

};