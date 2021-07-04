#include "Model.h"

#include <fstream>
#include <iostream>

uint32_t Model::s_idCounter = 0;

Model::Model(const std::string &modelName, const std::string &modelPath) : m_uniqueName(modelName)
{
    m_workingPath = modelPath.substr(0, modelPath.find_last_of('/'));

    loadModel(modelPath);
    m_id = s_idCounter++;
}

Model::~Model()
{
    // Go through all loaded textures and delete them
    for (auto it = m_loadedTextures.begin(); it != m_loadedTextures.end(); it++) {
        delete (*it);
    }
}

void Model::draw(ShaderProgram *shaderProgram)
{
    if (!m_modelPrepared) {
        std::cout << "WARNING: Model not prepared! Unable to draw!" << std::endl;
        return;
    }

    // Iterate through every mesh and call the draw function
    for (auto mesh : m_meshes) {
        mesh->draw(shaderProgram);
    }
}

void Model::drawWithoutTextures()
{
    if (!m_modelPrepared) {
        std::cout << "WARNING: Model not prepared! Unable to draw!" << std::endl;
        return;
    }

    // Iterate through every mesh and call the draw function
    for (auto mesh : m_meshes) {
        mesh->drawWithoutTextures();
    }
}

void Model::loadModel(const std::string &pathToModel)
{
    std::ifstream input(pathToModel, std::ios::in | std::ios::binary);

    if (!input.is_open()) {
        std::cerr << "Could not find model file " << pathToModel << std::endl;
        return;
    }

    uint32_t numTextures;
    input.read((char *)&numTextures, sizeof(uint32_t));

    std::vector<TextureType> textureTypes;
    for (unsigned int i = 0; i < numTextures; i++) {
        TextureType currentTextureType;
        input.read((char *)&currentTextureType, sizeof(uint32_t));
        textureTypes.push_back(currentTextureType);
    }

    std::vector<std::string> textureSources;
    for (unsigned int i = 0; i < numTextures; i++) {
        std::string currentTextureSource;
        for (unsigned int i = 0; i < 128; i++) {
            uint8_t currentChar;
            input.read((char *)&currentChar, sizeof(uint8_t));

            if (currentChar) {
                currentTextureSource.push_back(currentChar);
            }
        }
        textureSources.push_back(currentTextureSource);
    }

    for (unsigned int i = 0; i < numTextures; i++) {
        TexturePrototype texture_prototype;
        std::string texturePath = m_workingPath + '/' + textureSources[i].c_str();

        texture_prototype.texturePath = std::move(texturePath);
        texture_prototype.textureType = textureTypes[i];

        m_modelTexturePrototypes.push_back(texture_prototype);
    }

    // When there is no normal map bound, please use fallback texture
    bool hasNormalMap = false;
    for (auto it = textureTypes.begin(); it != textureTypes.end(); it++) {
        if (*it == TextureType::Normal)
            hasNormalMap = true;
    }

    if (!hasNormalMap) {
        TexturePrototype texture_prototype;

        texture_prototype.texturePath = "data/res/models/tex/fallback_normal.png";
        texture_prototype.textureType = TextureType::Normal;

        m_modelTexturePrototypes.push_back(texture_prototype);
    }

    // Here starts the first mesh
    uint32_t numMeshes;
    input.read((char *)&numMeshes, sizeof(uint32_t));

    for (unsigned int j = 0; j < numMeshes; j++) {

        MeshPrototype mesh_prototype;

        uint32_t numMeshVertices, numMeshIndices, numMeshTextureIds;

        input.read((char *)&numMeshVertices, sizeof(uint32_t));
        input.read((char *)&numMeshIndices, sizeof(uint32_t));
        input.read((char *)&numMeshTextureIds, sizeof(uint32_t));

        uint32_t vertexBlockSize = numMeshVertices * sizeof(Vertex);
        uint32_t indexBlockSize = numMeshIndices * sizeof(uint32_t);

        // Here starts the first Vertex data

        std::vector<Vertex> meshVertices;
        meshVertices.resize(numMeshVertices);
        input.read((char *)meshVertices.data(), vertexBlockSize);
        mesh_prototype.meshVertices = std::move(meshVertices);

        std::vector<uint32_t> meshIndices;
        meshIndices.resize(numMeshIndices);
        input.read((char *)meshIndices.data(), indexBlockSize);
        mesh_prototype.meshIndices = std::move(meshIndices);

        for (unsigned int i = 0; i < numMeshTextureIds; i++) {
            uint32_t currentTextureId;
            input.read((char *)&currentTextureId, sizeof(uint32_t));
            mesh_prototype.textureIds.push_back(currentTextureId);
        }

        if (!hasNormalMap) {
            // This will be the last texture
            mesh_prototype.textureIds.push_back(numTextures);
        }

        m_modelMeshPrototypes.push_back(std::move(mesh_prototype));
    }

    input.close();
}

void Model::prepareModel()
{
    m_modelPrepared = true;

    // Create textures on GPU
    for (auto &it : m_modelTexturePrototypes) {
        Texture *newTex = new Texture(it.texturePath.c_str(), it.textureType);
        m_loadedTextures.push_back(newTex);
    }

    // Create meshes on GPU
    for (const auto &it : m_modelMeshPrototypes) {
        std::vector<Texture *> meshTextures;
        for (const auto it2 : it.textureIds) {
            meshTextures.push_back(m_loadedTextures[it2]);
        }

        Mesh *currentMesh = new Mesh(std::move(it.meshVertices), std::move(it.meshIndices), meshTextures);
        m_meshes.push_back(currentMesh);
    }
}

Mesh *Model::getMesh(unsigned int index)
{
    return m_meshes[index];
}

const std::string &Model::getUniqueName()
{
    return m_uniqueName;
}
