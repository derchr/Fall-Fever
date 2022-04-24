#pragma once

#include <glm/glm.hpp>

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