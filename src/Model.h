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
    const std::string &getUniqueName();

private:
    void loadModel(const std::string &pathToModel);

    std::vector<Mesh *> m_meshes;
    std::vector<Texture *> m_loadedTextures;

    std::vector<TexturePrototype> m_modelTexturePrototypes;
    std::vector<MeshPrototype> m_modelMeshPrototypes;

    std::string m_workingPath;

    bool m_modelPrepared = false;

    static uint32_t s_idCounter;
    uint32_t m_id;
    std::string m_uniqueName;
};
