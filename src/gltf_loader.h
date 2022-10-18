#pragma once

#include "image.h"
#include "material.h"
#include "mesh.h"
#include "transform.h"

#include <entt/entt.hpp>
#include <filesystem>
#include <fx/gltf.h>

static constexpr auto MAX_SIZE = 512 * 1024 * 1024;

struct GltfPrimitive
{
    entt::resource<Mesh> mesh;
    entt::resource<Material> material;
};

struct GltfMesh
{
    std::vector<GltfPrimitive> primitives;
};

struct GltfNode
{
    std::string name;
    Transform transform;
    std::optional<entt::resource<GltfMesh>> mesh;
    std::vector<entt::resource<GltfNode>> children;
};

struct GltfScene {
    std::vector<entt::resource<GltfNode>> nodes;
};

// Move to own file.
struct Gltf
{
    std::vector<entt::resource<Material>> materials;
    std::vector<entt::resource<GltfMesh>> meshes;
    std::vector<entt::resource<GltfNode>> nodes;
    std::vector<entt::resource<GltfScene>> scenes;

    entt::resource<GltfScene> default_scene;
    fx::gltf::Document document;
};

struct GltfLoader final
{
    using result_type = std::shared_ptr<Gltf>;

    auto operator()(std::filesystem::path const &document_path) -> result_type;

    entt::resource_cache<Image> &image_cache;
    entt::resource_cache<Material> &material_cache;
    entt::resource_cache<Mesh> &mesh_cache;

    entt::resource_cache<GltfMesh> &gltf_mesh_cache;
    entt::resource_cache<GltfNode> &gltf_node_cache;
    entt::resource_cache<GltfScene> &gltf_scene_cache;
};
