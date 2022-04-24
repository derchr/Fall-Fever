#pragma once

#include "Resource.h"

class AbstractTexture : public Resource, public GlResource
{
public:
    AbstractTexture(const std::string &path) : Resource(path)
    {}

protected:
    int32_t m_textureWidth;
    int32_t m_textureHeight;
    int32_t m_numComponents;
};
