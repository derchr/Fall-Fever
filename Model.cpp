#include "Model.h"

#include <iostream>

Model::Model(const char* pathToModel) {
    loadModel(pathToModel);
}

void Model::draw(ShaderProgram *shaderProgram) {

    // Iterate through every mesh and call the draw function
    for(auto it = meshes.begin(); it != meshes.end(); it++) {
        it->draw(shaderProgram);
    }

}

void Model::loadModel(std::string pathToModel) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(pathToModel, aiProcess_Triangulate | aiProcess_FlipUVs); //aiProcess_OptimizeMeshes ?

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = pathToModel.substr(0, pathToModel.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {

    // Push the node's meshes into the mesh vector
    for(uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Process child nodes too
    for(uint i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for(uint i = 0; i < mesh->mNumVertices; i++) {
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

        // Texture
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
    for(uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(uint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Material
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, texture_diffuse);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, texture_specular);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint8_t textureType) {

    std::vector<Texture> textures;
    for(uint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString filename;
        mat->GetTexture(type, i, &filename);

        bool skip = 0;
        for(uint j = 0; j < loadedTextures.size(); j++) {
            if(std::strcmp(loadedTextures[j].getPath().data(), filename.C_Str()) == 0) {
                textures.push_back(loadedTextures[j]);
                skip = 1;
                break;
            }
        }

        if(!skip) {
            std::string path = directory + '/' + filename.C_Str();
            Texture texture(path.c_str(), textureType);
            textures.push_back(texture);
        }
    }

    loadedTextures.clear();

    return textures;
}
