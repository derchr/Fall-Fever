#include "ShaderProgram.h"
#include "Texture.h"
#include "util/Log.h"

CubeMap::CubeMap(const std::string &texturePseudoPath)
{
    // Reserve space in vector so that elements can be accessed explicitly.
    m_texturePaths.resize(static_cast<int>(cubeMapFaces::CUBEMAP_FACES_NUM_ITEMS));
    m_textureBuffers.resize(static_cast<int>(cubeMapFaces::CUBEMAP_FACES_NUM_ITEMS));
    m_numComponents.resize(static_cast<int>(cubeMapFaces::CUBEMAP_FACES_NUM_ITEMS));

    fillTexturePathVector(texturePseudoPath);

    stbi_set_flip_vertically_on_load(0);

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int i = 0;
    for (auto &path : m_texturePaths) {

        int32_t numComponents;
        auto textureBuffer = stbi_load(path.c_str(), &m_textureWidth, &m_textureHeight, &numComponents, 0);

        if (!textureBuffer) {
            Log::logger().warn("CubeMap texture {} could not be loaded", path);
            return;
        }

        m_textureBuffers[i] = textureBuffer;
        m_numComponents[i] = numComponents;
        i++;
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
    // glDeleteTextures(1, &m_textureId);
}

void CubeMap::initializeOnGPU()
{
    if (m_isInitialized)
        return;

    m_isInitialized = true;

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int i = 0;
    for (auto &textureBuffer : m_textureBuffers) {
        GLenum internalFormat;
        GLenum dataFormat;

        switch (m_numComponents[i]) {
        case 1:
            internalFormat = GL_RED;
            dataFormat = GL_RED;
            break;
        case 3:
            internalFormat = GL_SRGB8;
            dataFormat = GL_RGB;
            break;
        case 4:
            internalFormat = GL_SRGB8_ALPHA8;
            dataFormat = GL_RGBA;
            break;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, m_textureWidth, m_textureHeight, 0,
                     dataFormat, GL_UNSIGNED_BYTE, textureBuffer);

        stbi_image_free(textureBuffer);
        i++;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

void CubeMap::fillTexturePathVector(const std::string &texturePseudoPath)
{
    for (unsigned int i = 0; i < static_cast<int>(cubeMapFaces::CUBEMAP_FACES_NUM_ITEMS); i++) {
        m_texturePaths[static_cast<int>(cubeMapFaces::cm_front)] = texturePseudoPath + "front.png";
        m_texturePaths[static_cast<int>(cubeMapFaces::cm_back)] = texturePseudoPath + "back.png";
        m_texturePaths[static_cast<int>(cubeMapFaces::cm_top)] = texturePseudoPath + "top.png";
        m_texturePaths[static_cast<int>(cubeMapFaces::cm_bottom)] = texturePseudoPath + "bottom.png";
        m_texturePaths[static_cast<int>(cubeMapFaces::cm_left)] = texturePseudoPath + "left.png";
        m_texturePaths[static_cast<int>(cubeMapFaces::cm_right)] = texturePseudoPath + "right.png";
    }
}

GLuint CubeMap::getTextureId() const
{
    return m_textureId;
}
