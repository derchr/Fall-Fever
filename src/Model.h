#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class Model
{
public:
    Model(std::string &modelName, std::string &pathToModel);
    ~Model();

    void draw(ShaderProgram *shaderProgram);
    void drawWithoutTextures();

    Mesh *getMesh(unsigned int index)
    {
        return meshes[index];
    }

    std::string getUniqueName()
    {
        return unique_name;
    }

private:
    void loadModel(std::string &pathToModel);

    std::vector<Mesh *> meshes;

    std::vector<Texture *> loadedTextures;

    std::string directory;

    static uint32_t id_counter;
    uint32_t id;
    std::string unique_name;

};
