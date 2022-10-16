#pragma once

#include "image.h"

#include <entt/entt.hpp>

struct Material
{
    entt::resource<Image> base_color_texture;
    entt::resource<Image> normal_map_texture;
};
