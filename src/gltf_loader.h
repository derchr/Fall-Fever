#pragma once

#include "image.h"
#include "material.h"
#include "mesh.h"

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

// Move to own file.
struct Gltf
{
    std::vector<entt::resource<Material>> materials;
    std::vector<entt::resource<GltfMesh>> meshes;
    fx::gltf::Document document;
};

struct GltfLoader final
{
    using result_type = std::shared_ptr<Gltf>;

    auto operator()(std::filesystem::path const &document_path) -> result_type;

    entt::resource_cache<Image> image_cache;
    entt::resource_cache<Material> material_cache;
    entt::resource_cache<Mesh> mesh_cache;
    entt::resource_cache<GltfMesh> gltf_mesh_cache;
};
