#include "Texture.h"
#include "ShaderProgram.h"
#include "util/Log.h"

Texture::Texture(const Prototype &texturePrototype)
    : m_texturePath(texturePrototype.texturePath), m_textureType(texturePrototype.textureType)
{
    stbi_set_flip_vertically_on_load(1);
    m_textureBuffer = stbi_load(m_texturePath.c_str(), &m_textureWidth, &m_textureHeight, &m_numComponents, 0);

    if (!m_textureBuffer)
        Log::logger().warn("Texture {} could not be loaded", m_texturePath);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureId);
}

void Texture::initializeOnGPU()
{
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
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_textureWidth, m_textureHeight, 0, dataFormat, GL_UNSIGNED_BYTE,
                 m_textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(m_textureBuffer);
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
    default:
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

GLuint Texture::textureId()
{
    return m_textureId;
}

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
