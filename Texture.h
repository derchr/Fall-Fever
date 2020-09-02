#pragma once

#include <cstdint>
#include <glad/glad.h>

class Texture {

public:

    Texture(const char* texturePath, GLuint shaderProgramId);
    ~Texture();

    void bind(uint8_t textureUnit);
    void unbind();

private:

    int32_t textureWidth;
    int32_t textureHeight;
    int32_t bitsPerPixel;

    GLuint textureId;

    GLuint shaderProgramId;

};