#pragma once

#include "image.h"
#include "shader.h"

#include <entt/entt.hpp>
#include <optional>

class Shader;

struct Material
{
    std::optional<entt::resource<Image>> base_color_texture;
    std::optional<entt::resource<Image>> normal_map_texture;

    static constexpr std::string_view SHADER_NAME{"standard_material"};
    entt::resource<Shader> shader;
};

struct GpuMaterial
{
    GpuMaterial(Material const &material);

    void bind() const;

    struct Binding
    {
        std::string uniform_name;
        int texture_unit;
    };

    std::optional<std::pair<GpuImage, Binding>> base_color_texture;
    std::optional<std::pair<GpuImage, Binding>> normal_map_texture;

    entt::resource<Shader> shader;
};
