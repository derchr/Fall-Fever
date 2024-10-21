#include "visibility.h"
#include "components/relationship.h"

void Visibility::propagate(entt::registry& registry)
{
    auto root_visibility_view = registry.view<Visibility>(entt::exclude<Parent>);
    auto visibility_view = registry.view<Visibility, Parent const>();

    for (auto [entity, visibility] : root_visibility_view.each()) {
        visibility.inherited_visibility = visibility.entity_visibility == EntityVisibility::Hidden
                                              ? InheritedVisibility::Hidden
                                              : InheritedVisibility::Visible;

        auto parent_inherited_visibility = visibility.inherited_visibility;
        if (auto* children = registry.try_get<Children>(entity)) {
            for (auto child : children->children) {
                std::function<void(entt::entity entity,
                                   InheritedVisibility parent_inherited_visibility)>
                    visibility_propagate =
                        [&](entt::entity entity, InheritedVisibility parent_inherited_visibility) {
                            auto [visibility, parent] = visibility_view.get(entity);
                            visibility.inherited_visibility =
                                visibility.entity_visibility == EntityVisibility::Hidden
                                    ? InheritedVisibility::Hidden
                                    : parent_inherited_visibility;

                            if (auto* children = registry.try_get<Children>(entity)) {
                                for (auto child : children->children) {
                                    visibility_propagate(child, visibility.inherited_visibility);
                                }
                            }
                        };

                visibility_propagate(child, parent_inherited_visibility);
            }
        }
    }
}
