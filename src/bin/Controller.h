#pragma once

#include "core/graphics/framebuffer.h"
#include "core/shader.h"
#include "entt/entity/fwd.hpp"
#include "scene/gltf_loader.h"
#include "input/input.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

class Scene;
class Camera;
class Window;
class Framebuffer;

class Controller
{
public:
    Controller();

    void run();

private:
    void recreate_framebuffer();

    std::shared_ptr<Window> m_gameWindow;
    std::shared_ptr<Scene> m_scene;

    Shader post_processing_shader{"post_processing", "data/shaders"};
    Framebuffer post_processing_framebuffer;

    entt::registry registry;

    entt::dispatcher event_dispatcher{};
    Input::KeyListener key_listener;
    Input::CursorListener cursor_listener;

    // Resource caches
    entt::resource_cache<Image> m_image_cache;
    entt::resource_cache<Material> m_material_cache;
    entt::resource_cache<Mesh> m_mesh_cache;
    entt::resource_cache<Scene> m_scene_cache;
    entt::resource_cache<Shader, ShaderLoader> m_shader_cache;
    entt::resource_cache<GltfMesh> m_gltf_mesh_cache;
    entt::resource_cache<GltfNode> m_gltf_node_cache;

    GltfLoader m_gltf_loader;
    entt::resource_cache<Gltf, GltfLoader> m_gltf_cache;
};
