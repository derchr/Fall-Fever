#include "scene.h"
#include "components/name.h"
#include "components/transform.h"
#include "core/camera.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/light.h"
#include "window/window.h"

Scene::Scene(entt::registry& registry) : registry(registry)
{
    auto mesh_view = registry.view<entt::resource<Mesh>>();
    for (auto [entity, mesh] : mesh_view.each()) {
        registry.emplace<GpuMesh>(entity, GpuMesh(mesh));

        // Remove Mesh resource as it is no longer needed.
        registry.erase<entt::resource<Mesh>>(entity);
    }

    auto material_view = registry.view<entt::resource<Material>>();
    for (auto [entity, material] : material_view.each()) {
        registry.emplace<GpuMaterial>(entity, GpuMaterial(material));

        // Remove Material resource as it is no longer needed.
        registry.erase<entt::resource<Material>>(entity);
    }

    // Spawn default lights
    auto directional_light = registry.create();
    registry.emplace<Name>(directional_light, "Directional Light");
    registry.emplace<Transform>(
        directional_light,
        Transform{.orientation = glm::toQuat(
                      glm::lookAt({}, DirectionalLight::DEFAULT_DIRECTION, Camera::UP_VECTOR))});
    registry.emplace<GlobalTransform>(directional_light, GlobalTransform{});
    registry.emplace<DirectionalLight>(
        directional_light, DirectionalLight{.illuminance = DirectionalLight::DEFAULT_ILLUMINANCE});

    auto point_light = registry.create();
    registry.emplace<Name>(point_light, "Point Light");
    registry.emplace<Transform>(point_light,
                                Transform{.translation = PointLight::DEFAULT_POSITION});
    registry.emplace<GlobalTransform>(point_light, GlobalTransform{});
    registry.emplace<PointLight>(point_light,
                                 PointLight{.intensity = PointLight::DEFAULT_INTENSITY});
}

void Scene::update()
{
    GlobalTransform::update(registry);
    Camera::aspect_ratio_update(registry);
    Camera::keyboard_movement(registry);

    if (registry.ctx().get<Window::MouseCatched>().catched) {
        Camera::mouse_orientation(registry);
    }
}
