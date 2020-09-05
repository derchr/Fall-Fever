 #pragma once

#include <glm/glm.hpp>

#define INIT_WINDOW_WIDTH 960
#define INIT_WINDOW_HEIGHT 720 
struct Vertex {
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

    // Color
    glm::vec4 color;
};
