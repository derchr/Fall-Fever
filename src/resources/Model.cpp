#include "Model.h"
#include "Texture.h"

Model::Model(std::string_view name, std::vector<Mesh> meshes) : m_meshes(std::move(meshes)), m_name(name)
{
}

void Model::draw(ShaderProgram const &shaderProgram) const
{
    // Iterate through every mesh and call the draw function
    for (const auto &mesh : m_meshes) {
        mesh.draw(shaderProgram);
    }
}

void Model::drawWithoutTextures() const
{
    // Iterate through every mesh and call the draw function
    for (const auto &mesh : m_meshes) {
        mesh.drawWithoutTextures();
    }
}

auto Model::getMesh(unsigned int index) -> Mesh *
{
    return &m_meshes[index];
}
