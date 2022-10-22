#include "scene.h"
#include "camera.h"
#include "mesh.h"
#include "name.h"
#include "relationship.h"
#include "shader.h"
#include "transform.h"
#include "util/Log.h"

Scene::Scene(entt::registry registry) : m_registry(std::move(registry))
{
    auto mesh_view = m_registry.view<entt::resource<Mesh>>();
    for (auto [entity, mesh] : mesh_view.each()) {
        m_registry.emplace<GpuMesh>(entity, GpuMesh(mesh));

        // Remove Mesh resource as it is no longer needed.
        m_registry.erase<entt::resource<Mesh>>(entity);
    }

    auto material_view = m_registry.view<entt::resource<Material>>();
    for (auto [entity, material] : material_view.each()) {
        m_registry.emplace<GpuMaterial>(entity, GpuMaterial(material));

        // Remove Material resource as it is no longer needed.
        m_registry.erase<entt::resource<Material>>(entity);
    }
}

void Scene::update(std::chrono::duration<float> delta,
                   KeyInput const &key_input,
                   MouseCursorInput const &mouse_cursor_input,
                   float aspect_ratio,
                   bool cursor_catched)
{
    {
        // Update GlobalTransform components
        // TODO: Only do this when the Transform changed.
        auto root_transform_view =
            m_registry.view<Transform const, GlobalTransform>(entt::exclude<Parent>);
        auto transform_view = m_registry.view<Transform const, GlobalTransform, Parent const>();

        for (auto [entity, transform, global_transform] : root_transform_view.each()) {
            global_transform = transform;

            auto parent_global_transform = global_transform;
            if (auto *children = m_registry.try_get<Children>(entity)) {
                for (auto child : children->children) {
                    std::function<void(entt::entity entity,
                                       GlobalTransform parent_global_transform)>
                        transform_propagate = [this, &transform_propagate, &transform_view](
                                                  entt::entity entity,
                                                  GlobalTransform parent_global_transform) {
                            auto [transform, global_transform, parent] = transform_view.get(entity);
                            global_transform.transform = parent_global_transform.transform *
                                                         GlobalTransform(transform).transform;

                            if (auto *children = m_registry.try_get<Children>(entity)) {
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

    Camera::aspect_ratio_update(m_registry, aspect_ratio);
    Camera::keyboard_movement(m_registry, key_input, delta);
    if (cursor_catched) {
        Camera::mouse_orientation(m_registry, mouse_cursor_input);
    }
}
