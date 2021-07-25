#include "Model.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <fstream>
#include <future>
#include <iostream>

uint32_t Model::s_idCounter = 0;

Model::Model(const Prototype &prototype) : m_id(s_idCounter++), m_uniqueName(prototype.modelName)
{
    m_workingPath = prototype.modelPath.substr(0, prototype.modelPath.find_last_of('/'));

    loadModel(prototype.modelPath);
}

void Model::initializeOnGPU()
{
    if (m_isInitialized)
        return;

    m_isInitialized = true;

    for (auto texture : m_textures)
        texture->initializeOnGPU();

    for (auto mesh : m_meshes)
        mesh->initializeOnGPU();
}

Model::~Model()
{
    // Go through all loaded textures and delete them
    for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
        delete (*it);
    }
}

void Model::draw(ShaderProgram *shaderProgram)
{
    // Iterate through every mesh and call the draw function
    for (auto mesh : m_meshes) {
        mesh->draw(shaderProgram);
    }
}

void Model::drawWithoutTextures()
{
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

    // Maybe write a texture loader class in future, that handles all this.
    {
        std::vector<std::future<void>> futures;
        std::mutex mutex;

        for (unsigned int i = 0; i < numTextures; i++) {
            std::string texturePath = m_workingPath + '/' + textureSources[i].c_str();
            Texture::Prototype texturePrototype{texturePath, textureTypes[i]};
            auto loadModel = [=, &mutex]() {
                Texture *currentTex = new Texture(texturePrototype.texturePath, texturePrototype.textureType);

                std::lock_guard<std::mutex> lock(mutex);
                m_textures.push_back(currentTex);
            };

            futures.push_back(std::async(std::launch::async, loadModel));
        }
    }

    // When there is no normal map bound, please use fallback texture
    bool hasNormalMap = false;
    for (auto it = textureTypes.begin(); it != textureTypes.end(); it++) {
        if (*it == TextureType::Normal)
            hasNormalMap = true;
    }

    if (!hasNormalMap) {
        Texture *currentTex = new Texture("data/res/models/tex/fallback_normal.png", TextureType::Normal);

        m_textures.push_back(currentTex);
    }

    // Here starts the first mesh
    uint32_t numMeshes;
    input.read((char *)&numMeshes, sizeof(uint32_t));

    for (unsigned int j = 0; j < numMeshes; j++) {
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

        std::vector<uint32_t> meshIndices;
        meshIndices.resize(numMeshIndices);
        input.read((char *)meshIndices.data(), indexBlockSize);

        std::vector<uint32_t> meshTextureIds;
        std::vector<Texture *> meshTextures;

        for (unsigned int i = 0; i < numMeshTextureIds; i++) {
            uint32_t currentTextureId;
            input.read((char *)&currentTextureId, sizeof(uint32_t));
            meshTextureIds.push_back(currentTextureId);
        }

        if (!hasNormalMap) {
            // This will be the last texture
            meshTextureIds.push_back(numTextures);
        }

        for (auto textureId : meshTextureIds) {
            meshTextures.push_back(m_textures[textureId]);
        }

        m_meshes.push_back(new Mesh(std::move(meshVertices), std::move(meshIndices), std::move(meshTextures)));
    }

    input.close();
}

Mesh *Model::getMesh(unsigned int index)
{
    return m_meshes[index];
}

const std::string &Model::getUniqueName()
{
    return m_uniqueName;
}
