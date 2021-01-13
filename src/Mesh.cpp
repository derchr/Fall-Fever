#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture *> textures) :
    numElements(indices.size()),
    textures(textures),
    vertexArray(static_cast<void *>(vertices.data()), static_cast<void *>(indices.data()), vertices.size(), indices.size())
{
    // Empty...
}

void Mesh::draw(ShaderProgram *shaderProgram)
{
    uint8_t typeNumberCount[TEXTURE_TYPE_NUM_ITEMS] {0};
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind all textures in order to its texture unit
    for (auto it = textures.begin(); it != textures.end(); it++) {
        int i = it - textures.begin();

        uint8_t currentTextureType = (*it)->getTextureType();

        (*it)->bind(i, shaderProgram, typeNumberCount[currentTextureType]);

        typeNumberCount[currentTextureType] += 1;
    }

    // Draw elements
    vertexArray.bind();
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
    vertexArray.unbind();

    // Unbind all textures
    for (auto it = textures.begin(); it != textures.end(); it++) {
        (*it)->unbind();
    }
}

void Mesh::drawWithoutTextures()
{
    vertexArray.bind();
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
    vertexArray.unbind();
}

VertexArray* Mesh::getVertexArray()
{
    return &vertexArray;
}
