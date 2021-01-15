#pragma once

#include "ShaderProgram.h"

#include "defines.h"

#include <cstdint>
#include <glad/glad.h>
#include <string>
#include <vector>

// Order is important!
enum cubeMapFaces {cm_right, cm_left, cm_top, cm_bottom, cm_back, cm_front, CUBEMAP_FACES_NUM_ITEMS};

class Texture
{
public:
    Texture(const char *texturePath, uint8_t textureType);
    ~Texture();

    void bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum);
    void unbind();

    uint8_t getTextureType();
    std::string getPath();
    GLuint getTextureId();

private:
    std::string texturePath;

    int32_t textureWidth;
    int32_t textureHeight;
    int32_t numComponents;

    GLuint textureId;

    uint8_t textureType;
};

class CubeMap
{
public:
    CubeMap(const char *texturePseudoPath);
    CubeMap(int RESOLUTION);

    ~CubeMap();

    void bind(ShaderProgram *shaderProgram);
    void unbind();

    GLuint getTextureId();

private:
    void fillTexturePathVector(const char *texturePseudoPath);

private:
    std::vector<std::string> texturePaths;

    GLuint textureId;

    int32_t textureWidth;
    int32_t textureHeight;
};
