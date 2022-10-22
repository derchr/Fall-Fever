#pragma once

#include "FrameBuffer.h"
#include "scene.h"
#include "shader.h"
#include "gltf_loader.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

class Window;
class Scene;
class Camera;
class Framebuffer;

class Controller
{
public:
    Controller();

    void run();

    void updateExposure(Shader &shader) const;

private:
    void limit_framerate();
    void update_window_dimensions();

    std::shared_ptr<Window> m_gameWindow;

    Shader skybox_shader{"skybox", "data/shaders"};
    Shader post_processing_shader{"post_processing", "data/shaders"};

    Framebuffer m_postProcessFrameBuffer;

    static constexpr unsigned MAX_FPS = 60;

    std::shared_ptr<Scene> m_scene;

    double m_deltaTime{};
    float m_exposure = 1.0;

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
