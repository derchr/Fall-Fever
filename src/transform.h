#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform
{
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale{1.0, 1.0, 1.0};
};

using GlobalTransform = Transform;
