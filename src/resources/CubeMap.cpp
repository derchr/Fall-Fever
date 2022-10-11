#include "CubeMap.h"
#include "../ShaderProgram.h"
#include "../util/Log.h"

#include <glad/gl.h>

TextureCubeMap::TextureCubeMap(const TextureCubeMapDescriptor &descriptor) : AbstractCubeMap(descriptor.path)
{
    stbi_set_flip_vertically_on_load(0);

    glGenTextures(1, &m_glId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::size_t faceCount = 0;
    for (const auto &faceName : FACE_NAMES) {
        std::string texturePath = descriptor.path + faceName;

        int textureWidth{};
        int textureHeight{};
        int numComponents{};

        auto *textureBuffer = stbi_load(texturePath.c_str(), &textureWidth, &textureHeight, &numComponents, 0);

        m_textureWidth = static_cast<unsigned>(textureWidth);
        m_textureHeight = static_cast<unsigned>(textureHeight);

        if (textureBuffer == nullptr) {
            Log::logger().warn("CubeMap texture {} could not be loaded", texturePath);
            return;
        }

        GLint internalFormat{};
        GLenum dataFormat{};

        switch (numComponents) {
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

        glTexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceCount), 0, internalFormat,
                     static_cast<GLsizei>(m_textureWidth), static_cast<GLsizei>(m_textureHeight), 0, dataFormat,
                     GL_UNSIGNED_BYTE, textureBuffer);

        stbi_image_free(textureBuffer);

        faceCount++;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void AbstractCubeMap::bind(ShaderProgram *shaderProgram) const
{
    std::string uniformName = "u_skybox";

    shaderProgram->setUniform(uniformName, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glId);
}

void AbstractCubeMap::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

InternalCubeMap::InternalCubeMap(unsigned int resolution) : AbstractCubeMap("internal")
{
    m_textureWidth = resolution;
    m_textureHeight = resolution;

    glGenTextures(1, &m_glId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < static_cast<int>(CubeMapFace::CUBEMAP_FACES_NUM_ITEMS); i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, static_cast<GLsizei>(resolution),
                     static_cast<GLsizei>(resolution), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
