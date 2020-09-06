#include "Model.h"

#include <iostream>

Model::Model(const char* pathToModel) {
    // Todo: check if model isn't already loaded --> will boost startup time drastically
    // actually all models should be loaded at startup and only a handle should be given to the entites...
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
    for(uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Process child nodes too
    for(uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture*> textures;

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
        std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, texture_diffuse);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, texture_specular);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture*> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, texture_normal);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture*> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, texture_height);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint8_t textureType) {

    std::vector<Texture*> textures;
    for(uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString filename;
        mat->GetTexture(type, i, &filename);

        std::string currentPath = directory + '/' + filename.C_Str();

        bool skip = 0;
        for(uint32_t j = 0; j < loadedTextures.size(); j++) {
            if(std::strcmp(loadedTextures[j]->getPath().c_str(), currentPath.c_str()) == 0) {
                textures.push_back(loadedTextures[j]);
                skip = 1;
                break;
            }
        }

        if(!skip) {
            Texture *texture = new Texture(currentPath.c_str(), textureType);
            loadedTextures.push_back(texture);

            // Add newest texture pointer to the mesh's texture-pointer vector
            Texture *new_tex = loadedTextures.back();
            textures.push_back(new_tex);
        }
    }

    return textures;
}
