#pragma once

#include <glad/glad.h>

#include "defines.h"

class VertexArray
{
public:
    VertexArray(void *vertexData, void *indexData, uint32_t numVertices, uint32_t numIndices);
    ~VertexArray();

    void bind();
    void unbind();
    
    static std::vector<Vertex> createVertices(float *vertices, uint32_t numVertices, float *textureCoordinates);

private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
};
