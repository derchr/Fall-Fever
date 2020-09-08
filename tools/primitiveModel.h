#pragma once

#include <vector>
#include <string>

struct Texture {

    std::string pathToTexture;
    uint32_t textureType;
    uint32_t textureId;

};

struct Mesh {

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> textureIds;

};

struct Model {

    std::vector<Texture> textures;
    std::vector<Mesh> meshes;
    std::string directory;

};
