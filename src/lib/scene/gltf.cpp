#include "gltf.h"
#include "components/name.h"
#include "components/relationship.h"
#include "core/camera.h"

#include <spdlog/spdlog.h>

auto Gltf::spawn_scene(std::size_t index,
                       entt::registry& registry,
                       entt::resource_cache<GltfNode>& node_cache) -> entt::entity
{
    if (document.scenes.size() <= index) {
        return entt::null;
    }

    auto& gltf_scene = document.scenes.at(index);

    entt::entity scene_entity = registry.create();
    registry.emplace<Transform>(scene_entity, Transform{});
    registry.emplace<GlobalTransform>(scene_entity, GlobalTransform{});
    registry.emplace<Children>(scene_entity, Children{});

    std::vector<entt::resource<GltfNode>> nodes;
    nodes.reserve(gltf_scene.nodes.size());

    for (auto node_id : gltf_scene.nodes) {
        auto const& node = document.nodes.at(node_id);
        entt::hashed_string node_hash(node.name.c_str());
        nodes.push_back(node_cache[node_hash]);
    }

    if (gltf_scene.name.empty()) {
        spdlog::warn("glTF scene has no name.");
    }

    // Spawn an entity for every node in scene
    for (auto const& node : nodes) {
        std::function<entt::entity(GltfNode const&, entt::entity)> spawn_node =
            [&registry, &spawn_node](GltfNode const& node, entt::entity parent) {
                auto entity = registry.create();
                registry.emplace<Name>(entity, node.name);
                registry.emplace<Transform>(entity, node.transform);
                registry.emplace<GlobalTransform>(entity, GlobalTransform{});
                registry.emplace<Parent>(entity, Parent{.parent = parent});

                std::vector<entt::entity> child_entities;

                auto mesh = node.mesh;
                if (mesh.has_value()) {
                    for (auto const& primitive : mesh.value()->primitives) {
                        auto mesh_entity = registry.create();
                        registry.emplace<Parent>(mesh_entity, Parent{.parent = entity});
                        registry.emplace<Transform>(mesh_entity, Transform{});
                        registry.emplace<GlobalTransform>(mesh_entity, GlobalTransform{});
                        registry.emplace<entt::resource<Mesh>>(mesh_entity, primitive.mesh);
                        registry.emplace<entt::resource<Material>>(mesh_entity, primitive.material);

                        child_entities.push_back(mesh_entity);
                    }
                }

                auto camera = node.camera;
                if (camera.has_value()) {
                    auto perspective =
                        std::get<fx::gltf::Camera::Perspective>(camera.value().projection);
                    Camera::Perspective camera_perspective{.fov = perspective.yfov,
                                                           .aspect_ratio = perspective.aspectRatio,
                                                           .near = perspective.znear,
                                                           .far = perspective.zfar};
                    registry.emplace<Camera>(entity, Camera{.projection = camera_perspective});
                }

                // Spawn child nodes
                for (auto const& child : node.children) {
                    auto child_entity = spawn_node(child, entity);
                    child_entities.push_back(child_entity);
                }

                registry.emplace<Children>(entity, Children{.children = child_entities});
                return entity;
            };

        auto node_entity = spawn_node(node, scene_entity);
        registry.get<Children>(scene_entity).children.push_back(node_entity);
    }

    return scene_entity;
}

auto Gltf::spawn_scene(std::string_view name,
                       entt::registry& registry,
                       entt::resource_cache<GltfNode>& node_cache) -> entt::entity
{
    auto it = std::find_if(document.scenes.cbegin(), document.scenes.cend(), [name](auto& scene) {
        return scene.name == name;
    });

    if (it != document.scenes.cend()) {
        auto index = std::distance(document.scenes.cbegin(), it);
        return spawn_scene(index, registry, node_cache);
    }

    return entt::null;
}

auto Gltf::spawn_default_scene(entt::registry& registry,
                               entt::resource_cache<GltfNode>& node_cache) -> entt::entity
{
    if (document.scene == -1) {
        return entt::null;
    }

    auto scene = spawn_scene(document.scene, registry, node_cache);

    // Convert meshes
    auto mesh_view = registry.view<entt::resource<Mesh>>();
    for (auto [entity, mesh] : mesh_view.each()) {
        registry.emplace<GpuMesh>(entity, GpuMesh(mesh));

        // Remove Mesh resource as it is no longer needed.
        registry.erase<entt::resource<Mesh>>(entity);
    }

    // Convert materials
    auto material_view = registry.view<entt::resource<Material>>();
    for (auto [entity, material] : material_view.each()) {
        registry.emplace<GpuMaterial>(entity, GpuMaterial(material));

        // Remove Material resource as it is no longer needed.
        registry.erase<entt::resource<Material>>(entity);
    }

    return scene;
}
