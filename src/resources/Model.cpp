#include "Model.h"
#include "../util/Log.h"
#include "ResourceHandler.h"
#include "Texture.h"

#include <fstream>
#include <future>

Model::Model(const ModelDescriptor &descriptor) : Resource(descriptor.path), NamedResource(descriptor.name)
{
    m_workingPath = descriptor.path.substr(0, descriptor.path.find_last_of('/'));

    loadModel(descriptor.path);
}

void Model::initialize()
{
    m_initialized = true;

    for (auto mesh : m_meshes)
        mesh->initializeOnGPU();
}

void Model::draw(ShaderProgram *shaderProgram) const
{
    // Iterate through every mesh and call the draw function
    for (const auto &mesh : m_meshes) {
        mesh->draw(shaderProgram);
    }
}

void Model::drawWithoutTextures() const
{
    // Iterate through every mesh and call the draw function
    for (const auto &mesh : m_meshes) {
        mesh->drawWithoutTextures();
    }
}

Mesh *Model::getMesh(unsigned int index) const
{
    return m_meshes[index];
}

void Model::loadModel(const std::string &pathToModel)
{
    std::ifstream input(pathToModel, std::ios::in | std::ios::binary);

    if (!input.is_open()) {
        Log::logger().warn("Could not find model file {}", pathToModel);
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
        for (unsigned int k = 0; k < 128; k++) {
            uint8_t currentChar;
            input.read((char *)&currentChar, sizeof(uint8_t));

            if (currentChar) {
                currentTextureSource.push_back(static_cast<char>(currentChar));
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

            auto loadModel = [=, this, &mutex]() {
                ResourceId texture = ResourceHandler::instance().registerResource<Texture>(
                    TextureDescriptor{texturePath, textureTypes[i]});

                std::lock_guard<std::mutex> lock(mutex);
                m_textures.push_back(texture);
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
        ResourceId texture = ResourceHandler::instance().registerResource<Texture>(
            TextureDescriptor{"data/res/models/tex/fallback_normal.png", TextureType::Normal});

        m_textures.push_back(texture);
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
        std::vector<ResourceId> meshTextures;

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
