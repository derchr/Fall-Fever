#pragma once

#include <glm/glm.hpp>

enum class TextureType
{
    Diffuse,
    Specular,
    Normal,
    Height,
    Gloss,
    TEXTURE_TYPE_NUM_ITEMS
};

struct Vertex
{
    // Postition
    glm::vec3 position;

    // UV Texture Mapping
    glm::vec2 textureCoords;

    // Normal vector
    glm::vec3 normalVec;

    // Tangent vector
    glm::vec3 tangentVec;

    // Bittangent vector
    glm::vec3 bitangentVec;
};