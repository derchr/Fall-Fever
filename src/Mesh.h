#pragma once

#include <vector>

#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Texture.h"
#include "defines.h"

class Mesh {

public:

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
    ~Mesh() = default;

    void draw(ShaderProgram *shaderProgram);

private:

    uint32_t numElements;
    std::vector<Texture*> textures;

    VertexArray vertexArray;

    void setupMesh();

};