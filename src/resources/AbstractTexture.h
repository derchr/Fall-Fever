#pragma once

#include "Resource.h"

class AbstractTexture : public Resource, public GlResource
{
public:
    AbstractTexture(const std::string &path) : Resource(path)
    {}

protected:
    uint32_t m_textureWidth;
    uint32_t m_textureHeight;
    uint32_t m_numComponents;
};
