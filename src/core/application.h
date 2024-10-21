#pragma once

#include "core/graphics/framebuffer.h"
#include "core/shader.h"
#include "entt/entity/fwd.hpp"
#include "entt/signal/fwd.hpp"
#include "input/input.h"
#include "scene/gltf_loader.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>

class Camera;
class Window;

namespace FeverCore {

class Application
{
public:
    Application();

    virtual ~Application();
    Application(Application const&) = delete;
    Application(Application&&) = delete;
    auto operator=(Application const&) -> Application& = delete;
    auto operator=(Application&&) -> Application& = delete;

    auto registry() -> entt::registry& { return entt_registry; }
    auto registry() const -> entt::registry const& { return entt_registry; }

    auto dispatcher() -> entt::dispatcher& { return event_dispatcher; }
    auto dispatcher() const -> entt::dispatcher const& { return event_dispatcher; }

    void run();

    virtual void update() {};
    virtual void renderUi() {};

protected:
    virtual void register_context_variables();
    void recreate_framebuffer();

    std::shared_ptr<Window> game_window;

    Shader post_processing_shader{"post_processing", "data/shaders"};
    Framebuffer post_processing_framebuffer;

    entt::registry entt_registry;

    entt::dispatcher event_dispatcher{};
    Input::KeyListener key_listener;
    Input::CursorListener cursor_listener;

    // Resource caches
    entt::resource_cache<Image> image_cache;
    entt::resource_cache<Material> material_cache;
    entt::resource_cache<Mesh> mesh_cache;
    entt::resource_cache<Shader, ShaderLoader> shader_cache;
    entt::resource_cache<GltfMesh> gltf_mesh_cache;
    entt::resource_cache<GltfNode> gltf_node_cache;

    GltfLoader gltf_loader;
    entt::resource_cache<Gltf, GltfLoader> gltf_cache;
};

} // namespace FeverCore
