#pragma once

#include "AbstractTexture.h"
#include "TextureType.h"

#include <string>
#include <stb/stb_image.h>

class ShaderProgram;

struct TextureDescriptor
{
    std::string path;
    TextureType textureType;
};

class Texture : public AbstractTexture
{
public:
    Texture(const TextureDescriptor &descriptor);

    TextureType textureType() const;

    void bind(uint8_t textureUnit, ShaderProgram *shaderProgram, uint8_t textureTypeNum) const;
    void unbind() const override;

protected:
    void initialize() override;

private:
    stbi_uc *m_textureBuffer;

    TextureType m_textureType;
};