#pragma once

#include <glad/glad.h>

#include "definitions/models.h"

class VertexArray
{
public:
    VertexArray(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices);
    ~VertexArray();

    void bind();
    void unbind();

    static std::vector<Vertex> createVertices(double *vertices, uint32_t numVertices, float *textureCoordinates);

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
};
