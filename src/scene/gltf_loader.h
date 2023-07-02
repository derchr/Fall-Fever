#pragma once

#include "gltf.h"

#include <entt/entt.hpp>
#include <filesystem>
#include <fx/gltf.h>

static constexpr auto MAX_SIZE = 512 * 1024 * 1024;

struct GltfLoader
{
    using result_type = std::shared_ptr<Gltf>;

    auto operator()(std::filesystem::path const& document_path) -> result_type;

    entt::resource_cache<Image>& image_cache;
    entt::resource_cache<Material>& material_cache;
    entt::resource_cache<Mesh>& mesh_cache;
    entt::resource_cache<Shader, ShaderLoader>& shader_cache;

    entt::resource_cache<GltfMesh>& gltf_mesh_cache;
    entt::resource_cache<GltfNode>& gltf_node_cache;
};
