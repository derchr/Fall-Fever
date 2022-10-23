#pragma once

#include <glm/glm.hpp>

struct Color
{
    double r{}, g{}, b{};
    double a = 1.0;
    bool linear = false;

    [[nodiscard]] auto to_vec3() const -> glm::vec3 { return {r, g, b}; }
};

constexpr auto operator*(Color const &color, float value) -> Color
{
    Color new_color = color;
    new_color.r *= value;
    new_color.g *= value;
    new_color.b *= value;
    return new_color;
}

namespace ColorConstant {
static constexpr Color WHITE{.r = 1.0, .g = 1.0, .b = 1.0};
static constexpr Color BLACK{.r = 0.0, .g = 0.0, .b = 0.0};
static constexpr Color RED{.r = 1.0, .g = 0.0, .b = 0.0};
static constexpr Color GREEN{.r = 0.0, .g = 1.0, .b = 0.0};
static constexpr Color BLUE{.r = 0.0, .g = 0.0, .b = 1.0};
} // namespace ColorConstant
