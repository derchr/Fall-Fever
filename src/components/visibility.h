#pragma once

#include <entt/entt.hpp>

struct Visibility
{
    enum class EntityVisibility
    {
        Inherited,
        Hidden,
        Visible
    } entity_visibility = EntityVisibility::Inherited;

    enum class InheritedVisibility
    {
        Hidden,
        Visible
    } inherited_visibility = InheritedVisibility::Visible;

    static void propagate(entt::registry& registry);
};
