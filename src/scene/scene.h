#pragma once

#include "gltf.h"

#include <chrono>
#include <entt/entt.hpp>

class Scene
{
public:
    Scene(entt::registry registry);

    void update();
    auto registry() -> auto & { return m_registry; }

private:
    entt::registry m_registry;
};
