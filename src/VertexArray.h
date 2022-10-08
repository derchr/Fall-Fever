#pragma once

#include <glad/gl.h>
#include <vector>

class VertexArray
{
public:
    VertexArray(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices);
    ~VertexArray();

    void bind();
    void unbind();

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
};
