#pragma once

#include "VertexArray.h"
#include "resources/Texture.h"

#include <vector>

class ShaderProgram;

class Mesh_
{
public:
    Mesh_(VertexArray vertexArray, std::vector<std::reference_wrapper<const Texture>> textures);

    void draw(ShaderProgram const &shaderProgram) const;
    void drawWithoutTextures() const;

    auto getVertexArray() -> VertexArray *;

private:
    VertexArray m_vertexArray;
    std::vector<std::reference_wrapper<const Texture>> m_textures;
};
