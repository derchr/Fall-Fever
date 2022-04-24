#pragma once

#include "VertexArray.h"
#include "definitions/models.h"
#include "resources/Texture.h"

#include <vector>

class ShaderProgram;

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<ResourceId> textures);
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
    std::vector<ResourceId> m_textures;

    VertexArray *m_vertexArray;
};
