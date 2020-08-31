#include <cstddef>

#include "VertexBuffer.h"
#include "defines.h"

VertexBuffer::VertexBuffer(void* data, uint32_t numVertices) {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(struct Vertex, x));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(struct Vertex, r));
    
    // This will also unbind the vertex buffer
    glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &bufferId);
}

void VertexBuffer::bind() {
    glBindVertexArray(VAO);
}

void VertexBuffer::unbind() {
    glBindVertexArray(0);
}
