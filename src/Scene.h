#pragma once

#include "gltf_loader.h"
#include "material.h"
#include "mesh.h"

#include <chrono>
#include <entt/entt.hpp>

class ShaderProgram;

class Scene
{
public:
    Scene();

    void update(std::chrono::duration<float> delta,
                ShaderProgram *shaderprogram,
                glm::mat4 viewProjMatrix,
                glm::vec3 viewPosition);

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
