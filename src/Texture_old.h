#pragma once

#include "definitions/models.h"
#include "resources/Texture.h"

#include <cstdint>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <vector>

class ShaderProgram;

// Order is important!
enum class cubeMapFaces
{
    cm_right,
    cm_left,
    cm_top,
    cm_bottom,
    cm_back,
    cm_front,
    CUBEMAP_FACES_NUM_ITEMS
};

class CubeMap
{
public:
    CubeMap(const std::string &texturePseudoPath);
    CubeMap(int RESOLUTION);

    void initializeOnGPU();

    ~CubeMap();

    void bind(ShaderProgram *shaderProgram);
    void unbind();

    GLuint getTextureId() const;

private:
    void fillTexturePathVector(const std::string &texturePseudoPath);

    bool m_isInitialized = false;

    std::vector<std::string> m_texturePaths;
    std::vector<stbi_uc *> m_textureBuffers;
    std::vector<int> m_numComponents;

    GLuint m_textureId;

    int32_t m_textureWidth;
    int32_t m_textureHeight;
};
