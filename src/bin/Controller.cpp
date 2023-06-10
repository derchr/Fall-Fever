#include "Controller.h"
#include "core/light.h"
#include "core/render.h"
#include "core/shader.h"
#include "core/time.h"
#include "input/input.h"
#include "scene/scene.h"
#include "window/window.h"

#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <filesystem>
#include <fx/gltf.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <thread>
#include <utility>

using namespace entt::literals;

Controller::Controller() :
    m_gameWindow(std::make_shared<Window>(event_dispatcher)),
    post_processing_framebuffer(m_gameWindow->physical_dimensions()),
    m_gltf_loader{.image_cache = m_image_cache,
                  .material_cache = m_material_cache,
                  .mesh_cache = m_mesh_cache,
                  .shader_cache = m_shader_cache,
                  .scene_cache = m_scene_cache,
                  .gltf_mesh_cache = m_gltf_mesh_cache,
                  .gltf_node_cache = m_gltf_node_cache,
                  .registry = registry},
    m_gltf_cache(m_gltf_loader),
    key_listener{.registry = registry},
    cursor_listener{.registry = registry}
{
    std::filesystem::path document_path("WaterBottle/glTF-Binary/WaterBottle.glb");
    entt::hashed_string document_hash(document_path.c_str());

    entt::resource<Gltf> gltf_document =
        m_gltf_cache.load(document_hash, document_path).first->second;

    m_scene = gltf_document->default_scene.value_or(gltf_document->scenes.at(0)).handle();

    Input::State<Input::KeyCode>::init_state(registry);

    event_dispatcher.sink<Window::ResizeEvent>().connect<&Controller::recreate_framebuffer>(this);
    event_dispatcher.sink<Input::KeyInput>().connect<&Input::KeyListener::key_event>(key_listener);
    event_dispatcher.sink<Input::MouseMotion>().connect<&Input::CursorListener::cursor_event>(cursor_listener);
}

void Controller::run()
{
    entt::hashed_string shader_hash(Material::SHADER_NAME.data());
    auto standard_material_shader =
        m_shader_cache.load(shader_hash, Material::SHADER_NAME).first->second;

    spdlog::info("Startup complete. Enter game loop.");

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->handle()) == GLFW_FALSE) {
        // --- Timing ---
        Time::update_delta_time(registry);

        // --- Check events, handle input ---
        // m_gameWindow->clear_mouse_cursor_input();
        glfwPollEvents();

        // --- Update game state ---
        event_dispatcher.update();

        m_gameWindow->update_descriptor(registry);
        m_gameWindow->mouse_catching(registry);
        m_gameWindow->close_on_esc(registry);

        m_scene->update();

        Input::State<Input::KeyCode>::update_state(registry);
        Input::reset_mouse_motion(registry);

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        post_processing_framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Light::update_lights(registry, standard_material_shader);
        Render::render(registry);

        Framebuffer::unbind();
        post_processing_framebuffer.draw(post_processing_shader);

        glfwSwapBuffers(&m_gameWindow->handle());
    }
}

void Controller::recreate_framebuffer()
{
    auto dimensions = m_gameWindow->physical_dimensions();
    post_processing_framebuffer = Framebuffer(dimensions);
}
