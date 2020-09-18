#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class Model {

public:

    Model(const char* pathToModel);
    ~Model();

    void draw(ShaderProgram *shaderProgram);

    Mesh * getMesh(unsigned int index) { return meshes[index]; }


private:

    void loadModel(std::string pathToModel);

    std::vector<Mesh*> meshes;

    std::vector<Texture*> loadedTextures;

    std::string directory;

};