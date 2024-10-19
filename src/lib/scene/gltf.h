#pragma once

#include "components/transform.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"

#include <entt/entt.hpp>
#include <fx/gltf.h>
#include <optional>
#include <vector>

struct GltfPrimitive
{
    entt::resource<Mesh> mesh;
    entt::resource<Material> material;
};

struct GltfMesh
{
    std::vector<GltfPrimitive> primitives;
};

struct GltfCamera
{
    std::variant<fx::gltf::Camera::Perspective, fx::gltf::Camera::Orthographic> projection;
};

struct GltfNode
{
    std::string name;
    Transform transform;
    std::optional<entt::resource<GltfMesh>> mesh;
    std::optional<GltfCamera> camera;
    std::vector<entt::resource<GltfNode>> children;
};

struct Gltf
{
    std::vector<entt::resource<Material>> materials;
    std::vector<entt::resource<GltfMesh>> meshes;
    std::vector<entt::resource<GltfNode>> nodes;

    fx::gltf::Document document;

    auto spawn_scene(std::size_t index,
                     entt::registry& registry,
                     entt::resource_cache<GltfNode>& node_cache) -> entt::entity;

    auto spawn_scene(std::string_view name,
                     entt::registry& registry,
                     entt::resource_cache<GltfNode>& node_cache) -> entt::entity;

    auto spawn_default_scene(entt::registry& registry, entt::resource_cache<GltfNode>& node_cache)
        -> entt::entity;
};
