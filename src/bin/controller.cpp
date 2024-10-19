#include "controller.h"
#include "components/name.h"
#include "components/transform.h"
#include "core/camera.h"
#include "core/light.h"
#include "spdlog/spdlog.h"
#include "window/window.h"

using namespace entt::literals;

Controller::Controller(std::string_view path)
{
    spdlog::info("Open {}", path);
    std::filesystem::path document_path(path);
    entt::hashed_string document_hash(document_path.string().c_str());

    entt::resource<Gltf> gltf_document =
        gltf_cache.load(document_hash, document_path).first->second;

    gltf_document->spawn_default_scene(registry(), gltf_node_cache);

    // Convert meshes
    auto mesh_view = registry().view<entt::resource<Mesh>>();
    for (auto [entity, mesh] : mesh_view.each()) {
        registry().emplace<GpuMesh>(entity, GpuMesh(mesh));

        // Remove Mesh resource as it is no longer needed.
        registry().erase<entt::resource<Mesh>>(entity);
    }

    // Convert materials
    auto material_view = registry().view<entt::resource<Material>>();
    for (auto [entity, material] : material_view.each()) {
        registry().emplace<GpuMaterial>(entity, GpuMaterial(material));

        // Remove Material resource as it is no longer needed.
        registry().erase<entt::resource<Material>>(entity);
    }

    // Spawn default lights
    auto directional_light = registry().create();
    registry().emplace<Name>(directional_light, "Directional Light");
    registry().emplace<Transform>(
        directional_light,
        Transform{.orientation = glm::toQuat(
                      glm::lookAt({}, DirectionalLight::DEFAULT_DIRECTION, Camera::UP_VECTOR))});
    registry().emplace<GlobalTransform>(directional_light, GlobalTransform{});
    registry().emplace<DirectionalLight>(
        directional_light, DirectionalLight{.illuminance = DirectionalLight::DEFAULT_ILLUMINANCE});

    auto point_light = registry().create();
    registry().emplace<Name>(point_light, "Point Light");
    registry().emplace<Transform>(point_light,
                                  Transform{.translation = PointLight::DEFAULT_POSITION});
    registry().emplace<GlobalTransform>(point_light, GlobalTransform{});
    registry().emplace<PointLight>(point_light,
                                   PointLight{.intensity = PointLight::DEFAULT_INTENSITY});
}

void Controller::update()
{
    Camera::keyboard_movement(registry());

    if (registry().ctx().get<Window::MouseCatched>().catched) {
        Camera::mouse_orientation(registry());
    }
}
