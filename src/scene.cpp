#include "scene.h"
#include "camera.h"
#include "light.h"
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

    // Spawn default lights
    auto directional_light = m_registry.create();
    m_registry.emplace<Name>(directional_light, "Directional Light");
    m_registry.emplace<Transform>(
        directional_light,
        Transform{.orientation = glm::toQuat(
                      glm::lookAt({}, DirectionalLight::DEFAULT_DIRECTION, Camera::UP_VECTOR))});
    m_registry.emplace<GlobalTransform>(directional_light, GlobalTransform{});
    m_registry.emplace<DirectionalLight>(
        directional_light, DirectionalLight{.illuminance = DirectionalLight::DEFAULT_ILLUMINANCE});

    auto point_light = m_registry.create();
    m_registry.emplace<Name>(point_light, "Point Light");
    m_registry.emplace<Transform>(point_light,
                                  Transform{.translation = PointLight::DEFAULT_POSITION});
    m_registry.emplace<GlobalTransform>(point_light, GlobalTransform{});
    m_registry.emplace<PointLight>(point_light,
                                   PointLight{.intensity = PointLight::DEFAULT_INTENSITY});
}

void Scene::update(std::chrono::duration<float> delta,
                   KeyInput const &key_input,
                   MouseCursorInput const &mouse_cursor_input,
                   float aspect_ratio,
                   bool cursor_catched)
{
    GlobalTransform::update(m_registry);
    Camera::aspect_ratio_update(m_registry, aspect_ratio);
    Camera::keyboard_movement(m_registry, key_input, delta);
    if (cursor_catched) {
        Camera::mouse_orientation(m_registry, mouse_cursor_input);
    }
}
