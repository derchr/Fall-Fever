#include "application.h"

#include "components/visibility.h"
#include "core/camera.h"
#include "core/light.h"
#include "core/render.h"
#include "core/shader.h"
#include "core/time.h"
#include "input/input.h"
#include "window/window.h"

#include <GLFW/glfw3.h>
#include <fx/gltf.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace FeverCore {

Application::~Application() = default;

Application::Application() :
    game_window(std::make_shared<Window>(event_dispatcher)),
    post_processing_framebuffer(game_window->physical_dimensions()),
    key_listener{.registry = entt_registry},
    cursor_listener{.registry = entt_registry},
    gltf_loader{.image_cache = image_cache,
                .material_cache = material_cache,
                .mesh_cache = mesh_cache,
                .shader_cache = shader_cache,
                .gltf_mesh_cache = gltf_mesh_cache,
                .gltf_node_cache = gltf_node_cache},
    gltf_cache(gltf_loader)
{
    register_context_variables();

    event_dispatcher.sink<Window::ResizeEvent>().connect<&Application::recreate_framebuffer>(this);
    event_dispatcher.sink<Input::KeyInput>().connect<&Input::KeyListener::key_event>(key_listener);
    event_dispatcher.sink<Input::MouseMotion>().connect<&Input::CursorListener::cursor_event>(
        cursor_listener);
}

void Application::run()
{
    entt::hashed_string shader_hash(Material::SHADER_NAME.data());
    auto standard_material_shader =
        shader_cache.load(shader_hash, Material::SHADER_NAME).first->second;

    spdlog::info("Startup complete. Enter game loop.");

    // This is the game loop
    while (glfwWindowShouldClose(&game_window->handle()) == GLFW_FALSE) {
        // --- Timing ---
        Time::update_delta_time(entt_registry);

        // --- Check events, handle input ---
        glfwPollEvents();

        // --- Update game state ---
        event_dispatcher.update();

        game_window->update_descriptor(entt_registry);
        game_window->mouse_catching(entt_registry);
        game_window->close_on_esc(entt_registry);

        GlobalTransform::propagate(entt_registry);
        Visibility::propagate(entt_registry);
        Camera::aspect_ratio_update(entt_registry);

        this->update();

        Input::State<Input::KeyCode>::update_state(entt_registry);
        Input::reset_mouse_motion(entt_registry);

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        post_processing_framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Light::update_lights(entt_registry, standard_material_shader);
        Render::render(entt_registry);

        Framebuffer::unbind();
        post_processing_framebuffer.draw(post_processing_shader);

        this->renderUi();

        glfwSwapBuffers(&game_window->handle());
    }
}

void Application::register_context_variables()
{
    entt_registry.ctx().emplace<Input::State<Input::KeyCode>>();
    entt_registry.ctx().emplace<Input::MouseMotion>();
}

void Application::recreate_framebuffer()
{
    auto dimensions = game_window->physical_dimensions();
    post_processing_framebuffer = Framebuffer(dimensions);
}

} // namespace FeverCore
