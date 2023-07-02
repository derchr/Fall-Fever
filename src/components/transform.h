#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform
{
    glm::vec3 translation{};
    glm::quat orientation{};
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
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0F), transform.translation);

        // Rotate
        glm::mat4 rotation_matrix = glm::toMat4(transform.orientation);

        // Scale
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0F), transform.scale);

        this->transform = translation_matrix * rotation_matrix * scale_matrix;
    }

    glm::mat4 transform{};

    [[nodiscard]] auto position() const -> glm::vec3 { return transform[3]; };

    static void update(entt::registry &registry);
};
