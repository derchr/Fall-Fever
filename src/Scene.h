#pragma once

#include "Window.h"
#include "gltf_loader.h"
#include "material.h"
#include "mesh.h"

#include <chrono>
#include <entt/entt.hpp>

class Shader;

class Scene
{
public:
    Scene(entt::resource_cache<Shader, ShaderLoader> &shader_cache);

    void update(std::chrono::duration<float> delta_time,
                KeyInput const &key_input,
                MouseCursorInput const &mouse_cursor_input,
                float aspect_ratio,
                bool cursor_catched);

    auto registry() -> entt::registry & { return m_registry; }

private:
    entt::registry m_registry;

    // Resource caches
    entt::resource_cache<Image> m_image_cache;
    entt::resource_cache<Material> m_material_cache;
    entt::resource_cache<Mesh> m_mesh_cache;
    entt::resource_cache<GltfMesh> m_gltf_mesh_cache;
    entt::resource_cache<GltfNode> m_gltf_node_cache;
    entt::resource_cache<GltfScene> m_gltf_scene_cache; // May be moved out of Scene
};
