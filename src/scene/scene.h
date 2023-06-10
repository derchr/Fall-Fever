#pragma once

#include <chrono>
#include <entt/entt.hpp>

class Scene
{
public:
    Scene(entt::registry& registry);

    void update();

private:
    entt::registry& registry;
};
