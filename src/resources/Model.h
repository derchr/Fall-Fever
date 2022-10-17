#pragma once

#include "../Mesh.h"
#include "Resource.h"

#include <string>
#include <vector>

class Model
{
public:
    Model(std::string_view name, std::vector<Mesh_> meshes);

    void draw(ShaderProgram const &shaderProgram) const;
    void drawWithoutTextures() const;

    auto getMesh(unsigned int index) -> Mesh_ *; // TODO...

private:
    std::vector<Mesh_> m_meshes;
    std::vector<ResourceId> m_textures;

    std::string m_name;
};
