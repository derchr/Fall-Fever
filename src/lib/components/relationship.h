#pragma once

#include <entt/entt.hpp>

struct Parent
{
    entt::entity parent;
};

struct Children
{
    std::vector<entt::entity> children;
};
