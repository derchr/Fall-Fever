#pragma once

#include "VertexArray.h"
#include "resources/Resource.h"

#include <vector>

class ShaderProgram;

class Mesh
{
public:
    Mesh(VertexArray vertexArray, std::vector<ResourceId> textures);

    void draw(ShaderProgram const &shaderProgram) const;
    void drawWithoutTextures() const;

    auto getVertexArray() -> VertexArray *;

private:
    VertexArray m_vertexArray;
    std::vector<ResourceId> m_textures;
};
