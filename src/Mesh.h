#pragma once

#include <vector>

#include "ShaderProgram.h"
#include "Texture.h"
#include "VertexArray.h"
#include "definitions/models.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture *> textures);
    ~Mesh() = default;

    void draw(ShaderProgram *shaderProgram);
    void drawWithoutTextures();

    VertexArray *getVertexArray();

private:
    void setupMesh();

    uint32_t m_numElements;
    std::vector<Texture *> m_textures;

    VertexArray m_vertexArray;
};
