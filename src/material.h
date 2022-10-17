#pragma once

#include "image.h"

#include <entt/entt.hpp>
#include <optional>

struct Material
{
    std::optional<entt::resource<Image>> base_color_texture;
    std::optional<entt::resource<Image>> normal_map_texture;
};
