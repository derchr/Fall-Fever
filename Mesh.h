#pragma once

#include <vector>

#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "defines.h"

class Mesh {

public:

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
    ~Mesh() = default;

    void draw(ShaderProgram *shaderProgram);

private:

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture*> textures;

    VertexBuffer vertexBuffer;
    //ShaderProgram *shaderProgram;

    void setupMesh();

};