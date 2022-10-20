#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform
{
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale{1.0, 1.0, 1.0};
};

struct GlobalTransform
{
    GlobalTransform() = default;
    GlobalTransform(Transform const &transform)
    {
        // Translate * Rotate * Scale * vertex_vec;
        // First scaling, then rotation, then translation

        // Translate
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0F), transform.translation);

        // Rotate
        glm::mat4 rotationMatrix = glm::toMat4(transform.rotation);

        // Scale
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0F), transform.scale);

        this->transform = translationMatrix * rotationMatrix * scaleMatrix;
    }

    glm::mat4 transform{};
};
