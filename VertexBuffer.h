#pragma once

#include <glad/glad.h>

class VertexBuffer {

public:

    VertexBuffer(void* data, uint32_t numVertices);
    ~VertexBuffer();

    void bind();
    void unbind();

private:

    GLuint bufferId;
    GLuint VAO;

};