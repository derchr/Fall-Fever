#pragma once

#include "Texture.h"

#include <map>
#include <memory>

class TextureFactory // TODO not yet used
{
public:
    static TextureFactory &instance();

    void registerTexture(const Texture::Prototype &prototype);
    std::unique_ptr<Texture> texture(uint64_t resourceId);

private:
    std::map<uint64_t, std::unique_ptr<Texture>> m_textureMap;
};