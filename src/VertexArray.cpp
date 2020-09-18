#include <cstddef>

#include "VertexArray.h"
#include "defines.h"

VertexArray::VertexArray(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices) {

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(struct Vertex, position));

    // Normal vectors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(struct Vertex, normalVec));

    // UV Texture Mapping
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(struct Vertex, textureCoords));
    
    // This will also unbind the vertex buffer and index buffer
    glBindVertexArray(0);
}

VertexArray::~VertexArray() {
    glDeleteBuffers(1, &VBO);
}

void VertexArray::bind() {
    glBindVertexArray(VAO);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}
