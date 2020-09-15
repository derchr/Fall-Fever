#include "Model.h"

#include <iostream>
#include <fstream>

Model::Model(const char* pathToModel) {

    std::string modelSource = pathToModel;
    directory = modelSource.substr(0, modelSource.find_last_of('/'));

    loadModel(pathToModel);

}

Model::~Model() {
    // Go through all loaded textures and delete them
    for(auto it = loadedTextures.begin(); it != loadedTextures.end(); it++) {
        delete (*it);
    }
}

void Model::draw(ShaderProgram *shaderProgram) {

    // Iterate through every mesh and call the draw function
    for(auto it = meshes.begin(); it != meshes.end(); it++) {
        (*it)->draw(shaderProgram);
    }

}

void Model::loadModel(std::string pathToModel) {
    
    std::ifstream input(pathToModel, std::ios::in | std::ios::binary);

    if(!input.is_open()) {
		std::cout << "Could not find model file " << pathToModel << std::endl;
		return;
	}

    uint32_t numTextures;
    input.read((char*) &numTextures, sizeof(uint32_t));

    std::vector<uint32_t> textureTypes;
    for(unsigned int i = 0; i < numTextures; i++) {
        uint32_t currentTextureType;
        input.read((char*) &currentTextureType, sizeof(uint32_t));
        textureTypes.push_back(currentTextureType);
    }

    std::vector<std::string> textureSources;
    for(unsigned int i = 0; i < numTextures; i++) {
        std::string currentTextureSource;
        for(unsigned int i = 0; i < 128; i++) {
            uint8_t currentChar;
            input.read((char*) &currentChar, sizeof(uint8_t));

            if(currentChar)
            currentTextureSource.push_back(currentChar);
        }
        textureSources.push_back(currentTextureSource);
    }

    for(unsigned int i = 0; i < numTextures; i++) {
        std::string textureSource = directory + '/' + textureSources[i].c_str();
        Texture *newTex = new Texture(textureSource.c_str(), textureTypes[i]);
        loadedTextures.push_back(newTex);
    }

    // Here starts the first mesh
    uint32_t numMeshes;
    input.read((char*) &numMeshes, sizeof(uint32_t));

    for(unsigned int j = 0; j < numMeshes; j++) {

        uint32_t numMeshVertices, numMeshIndices, numMeshTextureIds;

        input.read((char*) &numMeshVertices, sizeof(uint32_t));
        input.read((char*) &numMeshIndices, sizeof(uint32_t));
        input.read((char*) &numMeshTextureIds, sizeof(uint32_t));

        uint32_t vertexBlockSize = numMeshVertices * sizeof(Vertex);
        uint32_t indexBlockSize = numMeshIndices * sizeof(uint32_t);

        // Here starts the first Vertex data

        std::vector<Vertex> meshVertices;
        meshVertices.resize(numMeshVertices);
        input.read((char*) meshVertices.data(), vertexBlockSize);

        std::vector<uint32_t> meshIndices;
        meshIndices.resize(numMeshIndices);
        input.read((char*) meshIndices.data(), indexBlockSize);

        std::vector<Texture*> meshTextures;
        for(unsigned int i = 0; i < numMeshTextureIds; i++) {
            uint32_t currentTextureId;
            input.read((char*) &currentTextureId, sizeof(uint32_t));
            meshTextures.push_back(loadedTextures[currentTextureId]);
        }

        Mesh *currentMesh = new Mesh(meshVertices, meshIndices, meshTextures);
        meshes.push_back(currentMesh);
    }

    input.close();

}
