#include "transform.h"
#include "relationship.h"

void GlobalTransform::update(entt::registry &registry)
{
    // Update GlobalTransform components
    // TODO: Only do this when the Transform changed.
    auto root_transform_view =
        registry.view<Transform const, GlobalTransform>(entt::exclude<Parent>);
    auto transform_view = registry.view<Transform const, GlobalTransform, Parent const>();

    for (auto [entity, transform, global_transform] : root_transform_view.each()) {
        global_transform = transform;

        auto parent_global_transform = global_transform;
        if (auto *children = registry.try_get<Children>(entity)) {
            for (auto child : children->children) {
                std::function<void(entt::entity entity, GlobalTransform parent_global_transform)>
                    transform_propagate =
                        [&registry, &transform_propagate, &transform_view](
                            entt::entity entity, GlobalTransform parent_global_transform) {
                            auto [transform, global_transform, parent] = transform_view.get(entity);
                            global_transform.transform = parent_global_transform.transform *
                                                         GlobalTransform(transform).transform;

                            if (auto *children = registry.try_get<Children>(entity)) {
                                for (auto child : children->children) {
                                    transform_propagate(child, global_transform);
                                }
                            }
                        };

                transform_propagate(child, parent_global_transform);
            }
        }
    }
}
