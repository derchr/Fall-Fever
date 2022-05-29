#pragma once

#include "AbstractTexture.h"

#include <array>
#include <stb/stb_image.h>
#include <vector>

// Order is important!
enum class CubeMapFace
{
    Right,
    Left,
    Top,
    Bottom,
    Back,
    Front,
    CUBEMAP_FACES_NUM_ITEMS
};

const std::array<std::string, 6> FACE_NAMES{"right.png", "left.png", "top.png", "bottom.png", "back.png", "front.png"};

class ShaderProgram;

struct TextureCubeMapDescriptor
{
    std::string path;
};

class AbstractCubeMap : public AbstractTexture
{
public:
    AbstractCubeMap(const std::string &path) : AbstractTexture(path)
    {}

    void bind(ShaderProgram *shaderProgram) const;
    void unbind() const override;
};

class TextureCubeMap : public AbstractCubeMap
{
public:
    TextureCubeMap(const TextureCubeMapDescriptor &descriptor);

protected:
    void initialize() override;

private:
    std::vector<stbi_uc *> m_textureBuffers;
};

class InternalCubeMap : public AbstractCubeMap
{
public:
    InternalCubeMap(unsigned int resolution);

protected:
    void initialize() override
    {}
};
