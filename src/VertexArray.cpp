#include <cstddef>
#include <vector>

#include "VertexArray.h"
#include "defines.h"

VertexArray::VertexArray(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indexData, GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(struct Vertex, position));

    // UV Texture Mapping
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(struct Vertex, textureCoords));

    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(struct Vertex, normalVec));

    // Tangent vectors
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(struct Vertex, tangentVec));

    // Bitangent vectors
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(struct Vertex, bitangentVec));

    // This will also unbind the vertex buffer and index buffer
    glBindVertexArray(0);
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &VBO);
}

void VertexArray::bind()
{
    glBindVertexArray(VAO);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

std::vector<Vertex> VertexArray::createVertices(double *vertices, uint32_t numVertices, float *textureCoordinates) {
    std::vector<Vertex> vertexVec;
    uint32_t i = 0;
    uint32_t k = 0;
    for(; i < numVertices; i+=3) {
        Vertex currentVertex = {};
        currentVertex.position.x = vertices[i];
        currentVertex.position.y = vertices[i+1];
        currentVertex.position.z = vertices[i+2];
        currentVertex.textureCoords.x = textureCoordinates[k];
        currentVertex.textureCoords.y = textureCoordinates[k+1];
        k+=2;
        vertexVec.push_back(currentVertex);
    }
    return vertexVec;
}
