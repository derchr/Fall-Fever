#pragma once

struct AttributeLocations
{
    int position = 0;
    int uv = 1;
    int normal = 2;
    int tangent = 3;
};

static constexpr AttributeLocations ATTRIBUTE_LOCATION;
