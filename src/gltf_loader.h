#pragma once

#include "image.h"
#include "material.h"

#include <entt/entt.hpp>
#include <filesystem>
#include <fx/gltf.h>

static constexpr auto MAX_SIZE = 512 * 1024 * 1024;

struct Gltf
{
    std::vector<entt::resource<Material>> materials;
};

struct gltf_loader final
{
    using result_type = std::shared_ptr<fx::gltf::Document>;

    auto operator()(std::filesystem::path const &document_path) -> result_type;

    entt::resource_cache<Image> image_cache;
};
