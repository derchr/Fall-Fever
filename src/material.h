#pragma once

#include "image.h"

#include <entt/entt.hpp>
#include <optional>

class ShaderProgram;

struct Material
{
    std::optional<entt::resource<Image>> base_color_texture;
    std::optional<entt::resource<Image>> normal_map_texture;
};

struct GpuMaterial
{
    GpuMaterial(Material const &material);

    void bind(ShaderProgram const &shader_program) const;

    struct Binding
    {
        std::string uniform_name;
        int texture_unit;
    };

    std::optional<std::pair<GpuImage, Binding>> base_color_texture;
    std::optional<std::pair<GpuImage, Binding>> normal_map_texture;
};
