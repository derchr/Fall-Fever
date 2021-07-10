#pragma once

#include "definitions/models.h"

#include <cstdint>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <vector>

class ShaderProgram;

// Order is important!
enum cubeMapFaces
{
    cm_right,
    cm_left,
    cm_top,
    cm_bottom,
    cm_back,
    cm_front,
    CUBEMAP_FACES_NUM_ITEMS
};

class Texture
{
public:
    struct Prototype
    {
        std::string texturePath;
        TextureType textureType;
    };

    Texture(const Prototype &prototype);
    ~Texture();

    void initializeOnGPU();

    void bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum);
    void unbind();

    TextureType getTextureType();
    std::string getPath();
    GLuint getTextureId();

private:
    bool m_isInitialized = false;

    std::string m_texturePath;

    stbi_uc *m_textureBuffer;

    int32_t m_textureWidth;
    int32_t m_textureHeight;
    int32_t m_numComponents;

    GLuint m_textureId;

    TextureType m_textureType;
};

class CubeMap
{
public:
    CubeMap(const char *texturePseudoPath);
    CubeMap(int RESOLUTION);

    void initializeOnGPU();

    ~CubeMap();

    void bind(ShaderProgram *shaderProgram);
    void unbind();

    GLuint getTextureId();

private:
    void fillTexturePathVector(const char *texturePseudoPath);

    bool m_isInitialized = false;

    std::vector<std::string> m_texturePaths;
    std::vector<stbi_uc *> m_textureBuffers;
    std::vector<int> m_numComponents;

    GLuint m_textureId;

    int32_t m_textureWidth;
    int32_t m_textureHeight;
};
