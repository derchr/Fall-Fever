#pragma once

#include "input.h"
#include "gltf.h"

#include <chrono>
#include <entt/entt.hpp>

class Shader;

class Scene
{
public:
    Scene(entt::registry registry);

    void update(std::chrono::duration<float> delta_time,
                KeyInput const &key_input,
                MouseCursorInput const &mouse_cursor_input,
                float aspect_ratio,
                bool cursor_catched);

    auto registry() -> entt::registry & { return m_registry; }

private:
    entt::registry m_registry;
};
