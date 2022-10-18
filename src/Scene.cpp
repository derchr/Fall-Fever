#include "Scene.h"
#include "name.h"
#include "transform.h"
#include "util/Log.h"

using namespace entt::literals;

// TODO: make scene initialization part of gltf loader as seen in bevy
Scene::Scene()
{
    GltfLoader loader{.image_cache = m_image_cache,
                      .material_cache = m_material_cache,
                      .mesh_cache = m_mesh_cache,
                      .gltf_mesh_cache = m_gltf_mesh_cache,
                      .gltf_node_cache = m_gltf_node_cache,
                      .gltf_scene_cache = m_gltf_scene_cache};

    entt::resource_cache<Gltf, GltfLoader> gltf_resource_cache{loader};

    std::filesystem::path document_path("ABeautifulGame.glb");
    // std::filesystem::path document_path("Lantern/glTF-Binary/Lantern.glb");
    entt::hashed_string document_hash(document_path.c_str());

    entt::resource<Gltf> gltf_document =
        gltf_resource_cache.load(document_hash, document_path).first->second;

    auto default_scene = gltf_document->default_scene;

    // Spawn an entity for every node in scene
    for (auto const &node : default_scene->nodes) {
        auto top_level_entity = m_registry.create();
        m_registry.emplace<Name>(top_level_entity, node->name);
        m_registry.emplace<Transform>(top_level_entity, node->transform);

        auto mesh = node->mesh;
        if (mesh.has_value()) {
            for (auto const &primitive : mesh.value()->primitives) {
                auto mesh_entity = m_registry.create();
                m_registry.emplace<Transform>(mesh_entity, Transform{});
                m_registry.emplace<entt::resource<Mesh>>(mesh_entity, primitive.mesh);
                m_registry.emplace<entt::resource<Material>>(mesh_entity, primitive.material);
            }
        }
    }

    auto name_view = m_registry.view<Name>();
    for (auto [entity, name] : name_view.each()) {
        Log::logger().info("Hello entity {}!", name);
    }

    auto mesh_view = m_registry.view<Mesh>();
        for (auto [entity, mesh] : mesh_view.each()) {
        Log::logger().info("Convert mesh {}!");
        
    }
}

void Scene::update(std::chrono::duration<float> delta)
{
}
