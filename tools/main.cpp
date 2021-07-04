/*
 
    This is the tool to generate model files for the Fall-Fever game. It's used to
    reduce loading time of the game, and also to reduce runtime dependencies (libassimp).
    
    The model files do not support material colors; only textures at this point.
    All path strings are 128 elements long.

*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>

#include "../src/definitions/models.h"
#include "primitiveModel.h"

void processNode(aiNode *node, const aiScene *scene, Model *model);
Mesh processMesh(aiMesh *mesh, const aiScene *scene, Model *model);
std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint8_t textureType, Mesh *mesh, Model *model);

int main(int argc, char** argv) {

    if(argc <= 1) {
        std::cout << "Usage: " << argv[0] << " <Modelfiles>" << std::endl;
    }

    std::vector<std::string> modelSources;

    for(int i = 0; i < argc - 1; i++) {
        modelSources.push_back(argv[i+1]);
    }

    Assimp::Importer importer;

    for(auto it = modelSources.begin(); it != modelSources.end(); it++) {
        unsigned int flags =
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_PreTransformVertices |
            aiProcess_GenNormals |
            aiProcess_OptimizeMeshes |
            aiProcess_OptimizeGraph |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality | 
            aiProcess_CalcTangentSpace;

        const aiScene* scene = importer.ReadFile((*it).c_str(), flags);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return 1;
        }
        
        Model currentModel;
        if(((*it).find('/')) < (*it).length()) {
            // source includes a /
            currentModel.m_workingPath = (*it).substr(0, (*it).find_last_of('/'));
        } else {
            currentModel.m_workingPath = ".";
        }

        processNode(scene->mRootNode, scene, &currentModel);

        std::string filenameWithoutExtension = (*it).substr(0, (*it).find_last_of('.'));
        std::string outputFilename = filenameWithoutExtension + ".ffo";

        std::ofstream output(outputFilename, std::ios::out | std::ios::binary);

        uint32_t numTextures = currentModel.textures.size();
        output.write((char*) &numTextures, sizeof(uint32_t));

        // Write texture types in order
        for(auto it1 = currentModel.textures.begin(); it1 != currentModel.textures.end(); it1++) {
            uint32_t currentTextureType = (*it1).textureType;
            output.write((char*) &currentTextureType, sizeof(uint32_t));
        }

        // Write texture sources
        for(auto it1 = currentModel.textures.begin(); it1 != currentModel.textures.end(); it1++) {
            for(unsigned int i = 0; i < 128; i++) {
                if(i < (*it1).pathToTexture.size()) {
                    uint8_t character = (*it1).pathToTexture[i];
                    output.write((char*) &character, sizeof(uint8_t));
                } else {
                    uint8_t character = 0;
                    output.write((char*) &character, sizeof(uint8_t));
                }
                
            }
        }

        // Write meshes
        uint32_t numMeshes = currentModel.m_meshes.size();
        output.write((char*) &numMeshes, sizeof(uint32_t));
        for (auto it1 = currentModel.m_meshes.begin(); it1 != currentModel.m_meshes.end(); it1++) {
            uint32_t numVertices = (*it1).vertices.size();
            uint32_t numIndices = (*it1).indices.size();
            uint32_t numTextureIds = (*it1).textureIds.size();

            output.write((char*) &numVertices, sizeof(uint32_t));
            output.write((char*) &numIndices, sizeof(uint32_t));
            output.write((char*) &numTextureIds, sizeof(uint32_t));

            Vertex *vertexData = (*it1).vertices.data();
            output.write((char*) vertexData, numVertices * sizeof(Vertex));

            uint32_t *indexData = (*it1).indices.data();
            output.write((char*) indexData, numIndices * sizeof(uint32_t));

            uint32_t *textureIdData = (*it1).textureIds.data();
            output.write((char*) textureIdData, numTextureIds * sizeof(uint32_t));
        }

        output.close();
    }

    return 0;
}

void processNode(aiNode *node, const aiScene *scene, Model* model) {

    // Push the node's meshes into the mesh vector
    for(uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->m_meshes.push_back(processMesh(mesh, scene, model));
    }

    // Process child nodes too
    for(uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, model);
    }

}

Mesh processMesh(aiMesh *mesh, const aiScene *scene, Model *model) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    Mesh currentMesh;

    for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Position
        glm::vec3 vector; 
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // Normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normalVec = vector;
        
        // Tangents
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangentVec = vector;

        // Bitangents
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.bitangentVec = vector;

        // Texture UV mapping
        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.textureCoords = vec;
        } else {
            vertex.textureCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Indices
    for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Material
    if(mesh->mMaterialIndex > 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE, static_cast<int>(TextureType::Diffuse), &currentMesh, model);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, static_cast<int>(TextureType::Specular), &currentMesh, model);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(
            material, aiTextureType_HEIGHT, static_cast<int>(TextureType::Normal), &currentMesh, model);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // Not entirely sure if aiTextureType_HEIGHT is correct
        std::vector<Texture> heightMaps = loadMaterialTextures(
            material, aiTextureType_HEIGHT, static_cast<int>(TextureType::Height), &currentMesh, model);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    currentMesh.vertices = vertices;
    currentMesh.indices = indices;

    return currentMesh;
}

std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint8_t textureType, Mesh *mesh, Model *model) {

    std::vector<Texture> textures;
    for(uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString filename;
        mat->GetTexture(type, i, &filename);

        std::string currentPath = model->m_workingPath + '/' + filename.C_Str();

        bool skip = 0;
        for(uint32_t j = 0; j < model->textures.size(); j++) {
            if(std::strcmp(model->textures[j].pathToTexture.c_str(), currentPath.c_str()) == 0) {
                textures.push_back(model->textures[j]);
                skip = 1;
                break;
            }
        }

        if(!skip) {
            Texture texture;
            texture.pathToTexture = currentPath;
            texture.textureType = textureType;
            // textureIds start at 0, but vector elements start at 1.
            texture.m_textureId = model->textures.size();

            model->textures.push_back(texture);

            // Add newest texture id to mesh
            mesh->textureIds.push_back(texture.m_textureId);
        }
    }

    return textures;
}
