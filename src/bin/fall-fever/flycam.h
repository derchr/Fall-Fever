#pragma once

#include <entt/entt.hpp>

struct Flycam
{
    static constexpr float SPEED = 0.5;
    static constexpr float ACCELERATION = 5.0;

    static void keyboard_movement(entt::registry& registry);
    static void mouse_orientation(entt::registry& registry);
};
