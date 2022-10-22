#pragma once

#include "transform.h"

#include <glm/glm.hpp>
#include <variant>

struct Camera
{
    static constexpr float DEFAULT_NEAR = 0.01;
    static constexpr float DEFAULT_FAR = 1000.0;
    static constexpr float DEFAULT_FOV = 90.0;

    static constexpr glm::vec3 UP_VECTOR = glm::vec3(0.0, 1.0, 0.0);

    struct Perspective
    {
        float fov = DEFAULT_FOV;
        float aspect_ratio;
        float near = DEFAULT_NEAR;
        float far = DEFAULT_FAR;

        float pitch{};
        float yaw{};
    };

    struct Orthographic
    {
        float left;
        float right;
        float top;
        float bottom;
        float near = DEFAULT_NEAR;
        float far = DEFAULT_FAR;
    };

    std::variant<Perspective, Orthographic> projection;

    [[nodiscard]] auto projection_matrix() const -> glm::mat4;
    [[nodiscard]] static auto view_matrix(GlobalTransform const &transform) -> glm::mat4;
    [[nodiscard]] static auto front_vector(GlobalTransform const &transform) -> glm::vec3;
};
