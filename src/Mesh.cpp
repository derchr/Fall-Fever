#include "Mesh.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "resources/ResourceHandler.h"
#include "resources/Texture.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<ResourceId> textures)
    : m_preInitializationVertexData{vertices, indices}, m_numElements(indices.size()), m_textures(textures)
{}

void Mesh::initializeOnGPU()
{
    m_vertexArray =
        new VertexArray(static_cast<void *>(m_preInitializationVertexData.vertices.data()),
                        static_cast<void *>(m_preInitializationVertexData.indices.data()),
                        m_preInitializationVertexData.vertices.size(), m_preInitializationVertexData.indices.size());
}

Mesh::~Mesh()
{
    delete m_vertexArray;
}

void Mesh::draw(ShaderProgram *shaderProgram)
{
    uint8_t typeNumberCount[static_cast<int>(TextureType::TEXTURE_TYPE_NUM_ITEMS)]{0};
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind all textures in order to its texture unit
    int i = 0;
    for (auto it : m_textures) {
        auto texture = std::static_pointer_cast<Texture>(ResourceHandler::instance().resource(it));
        TextureType currentTextureType = texture->textureType();

        texture->bind(i, shaderProgram, typeNumberCount[static_cast<int>(currentTextureType)]);

        typeNumberCount[static_cast<int>(currentTextureType)] += 1;

        i++;
    }

    // Draw elements
    m_vertexArray->bind();
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);
    m_vertexArray->unbind();

    // Unbind all textures
    for (auto it : m_textures) {
        auto texture = std::static_pointer_cast<Texture>(ResourceHandler::instance().resource(it));
        texture->unbind();
    }
}

void Mesh::drawWithoutTextures()
{
    m_vertexArray->bind();
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);
    m_vertexArray->unbind();
}

VertexArray *Mesh::getVertexArray()
{
    return m_vertexArray;
}
