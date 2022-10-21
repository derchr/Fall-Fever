#include "Scene.h"
#include "ShaderProgram.h"
#include "mesh.h"
#include "name.h"
#include "relationship.h"
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
        std::function<entt::entity(GltfNode const &, std::optional<entt::entity>)> spawn_node =
            [this, &spawn_node](GltfNode const &node, std::optional<entt::entity> parent) {
                auto entity = m_registry.create();
                m_registry.emplace<Name>(entity, node.name);
                m_registry.emplace<Transform>(entity, node.transform);
                m_registry.emplace<GlobalTransform>(entity, GlobalTransform{});

                if (parent.has_value()) {
                    m_registry.emplace<Parent>(entity, Parent{.parent = parent.value()});
                }

                std::vector<entt::entity> child_entities;

                auto mesh = node.mesh;
                if (mesh.has_value()) {
                    for (auto const &primitive : mesh.value()->primitives) {
                        auto mesh_entity = m_registry.create();
                        m_registry.emplace<Parent>(mesh_entity, Parent{.parent = entity});
                        m_registry.emplace<Transform>(mesh_entity, Transform{});
                        m_registry.emplace<GlobalTransform>(mesh_entity, GlobalTransform{});
                        m_registry.emplace<entt::resource<Mesh>>(mesh_entity, primitive.mesh);
                        m_registry.emplace<entt::resource<Material>>(mesh_entity,
                                                                     primitive.material);

                        child_entities.push_back(mesh_entity);
                    }
                }

                // Spawn child nodes
                for (auto const &child : node.children) {
                    auto child_entity = spawn_node(child, entity);
                    child_entities.push_back(child_entity);
                }

                m_registry.emplace<Children>(entity, Children{.children = child_entities});
                return entity;
            };

        spawn_node(node, {});
    }

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
                   ShaderProgram *shaderprogram,
                   glm::mat4 viewProjMatrix,
                   glm::vec3 viewPosition)
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
                std::function<void(entt::entity entity, GlobalTransform parent_global_transform)>
                    transform_propagate =
                        [this, &transform_propagate, &transform_view](
                            entt::entity entity, GlobalTransform parent_global_transform) {
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

    auto mesh_view = m_registry.view<GpuMesh const, GpuMaterial const, GlobalTransform const>();
    for (auto [entity, mesh, material, transform] : mesh_view.each()) {
        shaderprogram->bind();

        // Bind textures
        material.bind(*shaderprogram);

        // Bind modelview matrix uniform
        {
            glm::mat4 modelViewProj = viewProjMatrix * transform.transform;
            shaderprogram->setUniform("u_modelViewProjMatrix", modelViewProj);
            shaderprogram->setUniform("u_modelMatrix", transform.transform);
            shaderprogram->setUniform("u_viewPosition", viewPosition);
        }

        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indices_count, mesh.indices_type, nullptr);
        glBindVertexArray(0);

        ShaderProgram::unbind();
    }
}
