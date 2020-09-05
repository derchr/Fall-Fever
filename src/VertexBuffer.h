#pragma once

#include <glad/glad.h>

class VertexBuffer {

public:

    VertexBuffer(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices);
    ~VertexBuffer();

    void bind();
    void unbind();

private:

    GLuint bufferId;
    GLuint VAO;
    GLuint EBO;

};