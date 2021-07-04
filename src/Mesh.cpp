#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture *> textures)
    : m_numElements(indices.size()), m_textures(textures),
      m_vertexArray(static_cast<void *>(vertices.data()), static_cast<void *>(indices.data()), vertices.size(),
                    indices.size())
{}

void Mesh::draw(ShaderProgram *shaderProgram)
{
    uint8_t typeNumberCount[static_cast<int>(TextureType::TEXTURE_TYPE_NUM_ITEMS)]{0};
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind all textures in order to its texture unit
    for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
        const int i = it - m_textures.begin();

        TextureType currentTextureType = (*it)->getTextureType();

        (*it)->bind(i, shaderProgram, typeNumberCount[static_cast<int>(currentTextureType)]);

        typeNumberCount[static_cast<int>(currentTextureType)] += 1;
    }

    // Draw elements
    m_vertexArray.bind();
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);
    m_vertexArray.unbind();

    // Unbind all textures
    for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
        (*it)->unbind();
    }
}

void Mesh::drawWithoutTextures()
{
    m_vertexArray.bind();
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);
    m_vertexArray.unbind();
}

VertexArray *Mesh::getVertexArray()
{
    return &m_vertexArray;
}
