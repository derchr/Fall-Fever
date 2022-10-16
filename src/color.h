#pragma once

struct Color
{
    double r{}, g{}, b{};
    double a = 1.0;
    bool linear = false;
};

namespace ColorConstant {
static constexpr Color WHITE{.r = 1.0, .g = 1.0, .b = 1.0};
static constexpr Color BLACK{.r = 0.0, .g = 0.0, .b = 0.0};
static constexpr Color RED{.r = 1.0, .g = 0.0, .b = 0.0};
static constexpr Color GREEN{.r = 0.0, .g = 1.0, .b = 0.0};
static constexpr Color BLUE{.r = 0.0, .g = 0.0, .b = 1.0};
} // namespace ColorConstant
