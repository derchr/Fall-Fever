#pragma once

#include "../Mesh.h"
#include "Resource.h"

#include <string>
#include <vector>

struct ModelDescriptor
{
    std::string path;
    std::string name;
};

class Model : public Resource, public NamedResource
{
public:
    Model(const ModelDescriptor &descriptor);

    void draw(ShaderProgram *shaderProgram) const;
    void drawWithoutTextures() const;

    Mesh *getMesh(unsigned int index) const; // TODO...

protected:
    void initialize() override;

private:
    void loadModel(const std::string &pathToModel);

    std::vector<Mesh *> m_meshes;
    std::vector<ResourceId> m_textures;

    std::string m_workingPath;
};
