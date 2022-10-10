#include "Mesh.h"

#include "ShaderProgram.h"
#include "VertexArray.h"
#include "resources/ResourceHandler.h"
#include "resources/Texture.h"
#include <utility>

Mesh::Mesh(VertexArray vertexArray, std::vector<ResourceId> textures)
    : m_vertexArray(std::move(vertexArray)), m_textures(std::move(textures))
{
}

void Mesh::draw(ShaderProgram const &shaderProgram) const
{
    std::array<uint8_t, static_cast<std::size_t>(TextureType::TEXTURE_TYPE_NUM_ITEMS)> typeNumberCount{};

    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind all textures in order to its texture unit
    std::size_t textureNum = 0;
    for (auto textureIt : m_textures) {
        auto texture = std::static_pointer_cast<Texture>(ResourceHandler::instance().resource(textureIt));
        TextureType currentTextureType = texture->textureType();

        texture->bind(static_cast<uint8_t>(textureNum), shaderProgram,
                      typeNumberCount.at(static_cast<std::size_t>(currentTextureType)));

        typeNumberCount.at(static_cast<std::size_t>(currentTextureType)) += 1;

        textureNum++;
    }

    // Draw elements
    m_vertexArray.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_vertexArray.indicesCount()),
                   static_cast<GLenum>(m_vertexArray.indicesType()), nullptr);
    VertexArray::unbind();

    // Unbind all textures
    for (auto textureIt : m_textures) {
        auto texture = std::static_pointer_cast<Texture>(ResourceHandler::instance().resource(textureIt));
        texture->unbind();
    }
}

void Mesh::drawWithoutTextures() const
{
    m_vertexArray.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_vertexArray.indicesCount()),
                   static_cast<GLenum>(m_vertexArray.indicesType()), nullptr);
    VertexArray::unbind();
}

auto Mesh::getVertexArray() -> VertexArray *
{
    return &m_vertexArray;
}
