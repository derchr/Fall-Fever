#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

#include "Mesh.h"

class Model {

public:

    Model(const char* pathToModel);
    ~Model() = default;

    void draw(ShaderProgram *shaderProgram);


private:

    void loadModel(std::string pathToModel);

    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint8_t textureType);


    std::vector<Mesh> meshes;

    std::vector<Texture> loadedTextures;

    void textureFromFile(aiMaterial *mat, aiTextureType type);

    std::string directory;
};