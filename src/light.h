#pragma once

#include "color.h"
#include "shader.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct PointLight
{
    static constexpr glm::vec3 DEFAULT_POSITION{4.0, 1.0, 6.0};
    static constexpr float DEFAULT_INTENSITY = 3.0;

    Color color = ColorConstant::WHITE;
    float intensity{};
};

struct DirectionalLight
{
    static constexpr glm::vec3 DEFAULT_DIRECTION{-0.2, -1.0, -0.3};
    static constexpr float DEFAULT_ILLUMINANCE = 5.0;

    Color color = ColorConstant::WHITE;
    float illuminance{};
};

namespace Light {
void update_lights(entt::registry &registry, Shader &shader);
}
