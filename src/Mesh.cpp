#include "Mesh.h"

#include "ShaderProgram.h"
#include "VertexArray.h"
#include "resources/Texture.h"
#include <utility>

Mesh::Mesh(VertexArray vertexArray, std::vector<std::reference_wrapper<const Texture>> textures)
    : m_vertexArray(std::move(vertexArray)), m_textures(std::move(textures))
{
}

void Mesh::draw(ShaderProgram const &shaderProgram) const
{
    // Bind all textures in order to its texture unit
    std::size_t textureNum = 0;
    for (auto textureIt : m_textures) {
        textureIt.get().bind(static_cast<uint8_t>(textureNum), shaderProgram);
        textureNum++;
    }

    // Draw elements
    m_vertexArray.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_vertexArray.indicesCount()),
                   static_cast<GLenum>(m_vertexArray.indicesType()), nullptr);
    VertexArray::unbind();

    // Unbind all textures
    for (auto textureIt : m_textures) {
        textureIt.get().unbind();
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
