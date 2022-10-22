#include "Scene.h"
#include "ShaderProgram.h"
#include "camera.h"
#include "mesh.h"
#include "name.h"
#include "relationship.h"
#include "transform.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <algorithm>

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

    // Spawn the camera
    auto entity = m_registry.create();
    m_registry.emplace<Name>(entity, "Camera");
    m_registry.emplace<Transform>(entity, Transform{.translation = glm::vec3(0.0, 0.5, -2.0)});
    m_registry.emplace<GlobalTransform>(entity, GlobalTransform{});
    m_registry.emplace<Camera>(entity,
                               Camera{.projection = Camera::Perspective{.aspect_ratio = 1.6}});
}

void Scene::update(std::chrono::duration<float> delta,
                   ShaderProgram *shaderprogram,
                   KeyInput const &key_input,
                   MouseCursorInput const &mouse_cursor_input,
                   float aspect_ratio)
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

    {
        auto mesh_view = m_registry.view<GpuMesh const, GpuMaterial const, GlobalTransform const>();
        auto camera_view = m_registry.view<Camera const, GlobalTransform const>();
        auto camera_entity = camera_view.front();
        auto [camera, camera_transform] = camera_view.get(camera_entity);
        glm::mat4 view_projection_matrix =
            camera.projection_matrix() * Camera::view_matrix(camera_transform);

        for (auto [entity, mesh, material, transform] : mesh_view.each()) {
            shaderprogram->bind();

            // Bind textures
            material.bind(*shaderprogram);

            // Bind modelview matrix uniform
            {
                glm::mat4 modelViewProj = view_projection_matrix * transform.transform;
                shaderprogram->setUniform("u_modelViewProjMatrix", modelViewProj);
                shaderprogram->setUniform("u_modelMatrix", transform.transform);
                shaderprogram->setUniform("u_viewPosition", camera_transform.position());
            }

            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indices_count, mesh.indices_type, nullptr);
            glBindVertexArray(0);

            ShaderProgram::unbind();
        }
    }

    // Camera keyboard update
    {
        auto camera_view = m_registry.view<Camera const, Transform, GlobalTransform const>();
        auto camera_entity = camera_view.front();
        auto [camera, camera_transform, camera_global_transform] = camera_view.get(camera_entity);

        glm::vec3 front_vec = Camera::front_vector(camera_global_transform);
        glm::vec3 front_vec_without_y = glm::vec3(front_vec.x, 0., front_vec.z);
        glm::vec3 deltaPos = glm::vec3(0., 0., 0.);
        // float deltaFactor = SPEED * deltaTime * (m_accellerate ? 5.0 : 1.0);
        float delta_factor = 0.5 * delta.count();
        // m_accellerate = false;

        for (auto const &[key, pressed] : key_input) {
            if (key == GLFW_KEY_W && pressed) {
                deltaPos += delta_factor * glm::normalize(front_vec_without_y);
            }
            if (key == GLFW_KEY_S && pressed) {
                deltaPos -= delta_factor * glm::normalize(front_vec_without_y);
            }
            if (key == GLFW_KEY_A && pressed) {
                deltaPos -= delta_factor * glm::normalize(glm::cross(front_vec, Camera::UP_VECTOR));
            }
            if (key == GLFW_KEY_D && pressed) {
                deltaPos += delta_factor * glm::normalize(glm::cross(front_vec, Camera::UP_VECTOR));
            }
            if (key == GLFW_KEY_SPACE && pressed) {
                deltaPos += delta_factor * Camera::UP_VECTOR;
            }
            if (key == GLFW_KEY_LEFT_SHIFT && pressed) {
                deltaPos -= delta_factor * Camera::UP_VECTOR;
            }
            // if (key == GLFW_KEY_LEFT_ALT && pressed) {
            //     m_accellerate = true;
            // }
        }
        camera_transform.translation += deltaPos;
    }

    // Camera mouse update
    {
        auto camera_view = m_registry.view<Camera, Transform>();
        auto camera_entity = camera_view.front();
        auto [camera, camera_transform] = camera_view.get(camera_entity);

        auto [deltaX, deltaY] = mouse_cursor_input;

        if (std::abs(deltaX) < std::numeric_limits<double>::epsilon() &&
            std::abs(deltaY) < std::numeric_limits<double>::epsilon()) {
            return;
        }

        auto pitch = static_cast<float>(deltaY);
        auto yaw = static_cast<float>(deltaX);
        auto roll = 0.0F;

        // Orthographic projection currently unsupported
        auto &camera_perspective = std::get<Camera::Perspective>(camera.projection);

        camera_perspective.pitch += glm::radians(pitch);
        camera_perspective.yaw += glm::radians(yaw);

        static constexpr float PITCH_CLIP = glm::radians(89.);
        camera_perspective.pitch =
            std::clamp(static_cast<float>(camera_perspective.pitch), -PITCH_CLIP, PITCH_CLIP);

        camera_transform.orientation =
            glm::quat(glm::vec3(-camera_perspective.pitch, -camera_perspective.yaw, 0.0));
    }

    // Camera aspect ratio update
    {
        auto camera_view = m_registry.view<Camera>();
        auto camera_entity = camera_view.front();
        auto [camera] = camera_view.get(camera_entity);

        // Orthographic projection currently unsupported
        auto &camera_perspective = std::get<Camera::Perspective>(camera.projection);
        camera_perspective.aspect_ratio = aspect_ratio;
    }
}