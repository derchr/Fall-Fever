#pragma once

#include <glad/glad.h>

class VertexArray {

public:

    VertexArray(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices);
    ~VertexArray();

    void bind();
    void unbind();

private:

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

};