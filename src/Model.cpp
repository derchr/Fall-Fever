#include "Model.h"

#include <iostream>
#include <fstream>

uint32_t Model::id_counter = 0;

Model::Model(const std::string &modelName, const std::string &modelPath) :
    unique_name(modelName)
{
    directory = modelPath.substr(0, modelPath.find_last_of('/'));

    loadModel(modelPath);
    id = id_counter++;
}

Model::~Model()
{
    // Go through all loaded textures and delete them
    for (auto it = loadedTextures.begin(); it != loadedTextures.end(); it++) {
        delete (*it);
    }
}

void Model::draw(ShaderProgram *shaderProgram)
{
    if (!model_prepared) {
        std::cout << "WARNING: Model not prepared! Unable to draw!" << std::endl;
        return;
    }

    // Iterate through every mesh and call the draw function
    for (auto it = meshes.begin(); it != meshes.end(); it++) {
        (*it)->draw(shaderProgram);
    }
}

void Model::drawWithoutTextures()
{
    if (!model_prepared) {
        std::cout << "WARNING: Model not prepared! Unable to draw!" << std::endl;
        return;
    }

    // Iterate through every mesh and call the draw function
    for (auto it = meshes.begin(); it != meshes.end(); it++) {
        (*it)->drawWithoutTextures();
    }
}

void Model::loadModel(const std::string &pathToModel)
{
    std::ifstream input(pathToModel, std::ios::in | std::ios::binary);

    if (!input.is_open()) {
        std::cout << "Could not find model file " << pathToModel << std::endl;
        return;
    }

    uint32_t numTextures;
    input.read((char *) &numTextures, sizeof(uint32_t));

    std::vector<uint32_t> textureTypes;
    for (unsigned int i = 0; i < numTextures; i++) {
        uint32_t currentTextureType;
        input.read((char *) &currentTextureType, sizeof(uint32_t));
        textureTypes.push_back(currentTextureType);
    }

    std::vector<std::string> textureSources;
    for (unsigned int i = 0; i < numTextures; i++) {
        std::string currentTextureSource;
        for (unsigned int i = 0; i < 128; i++) {
            uint8_t currentChar;
            input.read((char *) &currentChar, sizeof(uint8_t));

            if (currentChar) {
                currentTextureSource.push_back(currentChar);
            }
        }
        textureSources.push_back(currentTextureSource);
    }


    for (unsigned int i = 0; i < numTextures; i++) {
        TexturePrototype texture_prototype;
        std::string texturePath = directory + '/' + textureSources[i].c_str();

        texture_prototype.texturePath = std::move(texturePath);
        texture_prototype.textureType = textureTypes[i];

        modelTexturePrototypes.push_back(texture_prototype);
    }

    // Here starts the first mesh
    uint32_t numMeshes;
    input.read((char *) &numMeshes, sizeof(uint32_t));

    for (unsigned int j = 0; j < numMeshes; j++) {

        MeshPrototype mesh_prototype;

        uint32_t numMeshVertices, numMeshIndices, numMeshTextureIds;

        input.read((char *) &numMeshVertices, sizeof(uint32_t));
        input.read((char *) &numMeshIndices, sizeof(uint32_t));
        input.read((char *) &numMeshTextureIds, sizeof(uint32_t));

        uint32_t vertexBlockSize = numMeshVertices * sizeof(Vertex);
        uint32_t indexBlockSize = numMeshIndices * sizeof(uint32_t);

        // Here starts the first Vertex data

        std::vector<Vertex> meshVertices;
        meshVertices.resize(numMeshVertices);
        input.read((char *) meshVertices.data(), vertexBlockSize);
        mesh_prototype.meshVertices = std::move(meshVertices);

        std::vector<uint32_t> meshIndices;
        meshIndices.resize(numMeshIndices);
        input.read((char *) meshIndices.data(), indexBlockSize);
        mesh_prototype.meshIndices = std::move(meshIndices);

        for (unsigned int i = 0; i < numMeshTextureIds; i++) {
            uint32_t currentTextureId;
            input.read((char *) &currentTextureId, sizeof(uint32_t));
            mesh_prototype.textureIds.push_back(currentTextureId);
        }

        modelMeshPrototypes.push_back(std::move(mesh_prototype));
    }

    input.close();
}

void Model::prepareModel()
{
    model_prepared = true;

    // Create textures on GPU
    for (auto& it : modelTexturePrototypes) {
        Texture *newTex = new Texture(it.texturePath.c_str(), it.textureType);
        loadedTextures.push_back(newTex);
    }

    // Create meshes on GPU
    for (const auto& it : modelMeshPrototypes) {
        std::vector<Texture *> meshTextures;
        for (const auto it2 : it.textureIds) {
            meshTextures.push_back(loadedTextures[it2]);
        }

        Mesh *currentMesh = new Mesh(std::move(it.meshVertices), std::move(it.meshIndices), meshTextures);
        meshes.push_back(currentMesh);
    }
}


Mesh* Model::getMesh(unsigned int index)
{
    return meshes[index];
}

std::string Model::getUniqueName()
{
    return unique_name;
}
