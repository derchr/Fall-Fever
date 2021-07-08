#pragma once

#include "definitions/models.h"

#include <vector>

class ShaderProgram;
class Texture;
class VertexArray;

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture *> textures);
    ~Mesh();

    void initializeOnGPU();

    void draw(ShaderProgram *shaderProgram);
    void drawWithoutTextures();

    VertexArray *getVertexArray();

private:
    struct PreInitializationVertexData
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    } m_preInitializationVertexData;

    bool m_isInitialized = false;

    uint32_t m_numElements;
    std::vector<Texture *> m_textures;

    VertexArray *m_vertexArray;
};
