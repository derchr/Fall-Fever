#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class Model
{
public:
    Model(const std::string& modelName, const std::string& pathToModel);
    ~Model();

    void draw(ShaderProgram *shaderProgram);
    void drawWithoutTextures();

    Mesh *getMesh(unsigned int index);
    std::string getUniqueName();

private:
    void loadModel(const std::string &pathToModel);

private:
    std::vector<Mesh *> meshes;
    std::vector<Texture *> loadedTextures;

    std::string directory;

    static uint32_t id_counter;
    uint32_t id;
    std::string unique_name;

};
