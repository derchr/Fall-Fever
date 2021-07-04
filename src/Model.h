#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

struct TexturePrototype
{
    TextureType textureType;
    std::string texturePath;
};

struct MeshPrototype
{
    std::vector<uint32_t> textureIds;
    std::vector<Vertex> meshVertices;
    std::vector<uint32_t> meshIndices;
};

class Model
{
public:
    Model(const std::string &modelName, const std::string &pathToModel);
    ~Model();

    void prepareModel();

    void draw(ShaderProgram *shaderProgram);
    void drawWithoutTextures();

    Mesh *getMesh(unsigned int index);
    std::string getUniqueName();

private:
    void loadModel(const std::string &pathToModel);

private:
    std::vector<Mesh *> meshes;
    std::vector<Texture *> loadedTextures;

    std::vector<TexturePrototype> modelTexturePrototypes;
    std::vector<MeshPrototype> modelMeshPrototypes;

    std::string directory;

    bool model_prepared = false;

    static uint32_t id_counter;
    uint32_t id;
    std::string unique_name;
};
