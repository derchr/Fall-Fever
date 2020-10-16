#pragma once

#include <glm/glm.hpp>

#define INIT_WINDOW_WIDTH 1280
#define INIT_WINDOW_HEIGHT 720

enum textureType {
    texture_diffuse,
    texture_specular,
    texture_normal,
    texture_height,
    texture_gloss,
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
