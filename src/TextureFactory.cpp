#include "TextureFactory.h"

TextureFactory &TextureFactory::instance()
{
    static TextureFactory instance;
    return instance;
}

void TextureFactory::registerTexture(const Texture::Prototype &prototype)
{
    auto texture = std::make_unique<Texture>(prototype);
    m_textureMap.insert({texture->textureId(), std::move(texture)});
}

std::unique_ptr<Texture> TextureFactory::texture(uint64_t resourceId)
{
    auto texture = std::move(m_textureMap.at(resourceId));
    texture->initializeOnGPU();
    return texture;
}
