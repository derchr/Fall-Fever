#pragma once

#include "definitions/models.h"
#include "VertexArray.h"

#include <vector>

class ShaderProgram;
class Texture;

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
