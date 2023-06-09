#include "Controller.h"
#include "core/light.h"
#include "core/render.h"
#include "core/shader.h"
#include "core/time.h"
#include "input/input.h"
#include "scene/gltf_loader.h"
#include "window/Window.h"

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

static constexpr unsigned MAX_FPS = 60;

Controller::Controller()
    : m_gameWindow(std::make_shared<Window>()),
      post_processing_framebuffer(m_gameWindow->physical_dimensions()),
      m_gltf_loader{.image_cache = m_image_cache,
                    .material_cache = m_material_cache,
                    .mesh_cache = m_mesh_cache,
                    .shader_cache = m_shader_cache,
                    .scene_cache = m_scene_cache,
                    .gltf_mesh_cache = m_gltf_mesh_cache,
                    .gltf_node_cache = m_gltf_node_cache},
      m_gltf_cache(m_gltf_loader)
{
    std::filesystem::path document_path("ABeautifulGame.glb");
    entt::hashed_string document_hash(document_path.c_str());

    entt::resource<Gltf> gltf_document =
        m_gltf_cache.load(document_hash, document_path).first->second;

    m_scene = gltf_document->default_scene.value_or(gltf_document->scenes.at(0)).handle();
}

void Controller::run()
{
    entt::hashed_string shader_hash(Material::SHADER_NAME.data());
    auto standard_material_shader =
        m_shader_cache.load(shader_hash, Material::SHADER_NAME).first->second;

    spdlog::info("Startup complete. Enter game loop.");

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->glfw_window()) == GLFW_FALSE) {
        // --- Timing ---
        Time::update_delta_time(m_scene->registry());

        // --- Check events, handle input ---
        m_gameWindow->clear_mouse_cursor_input();
        glfwPollEvents();

        Input::handle_keyboard_input(m_scene->registry(), m_gameWindow->key_input());
        Input::handle_mouse_cursor_input(m_scene->registry(), m_gameWindow->mouse_cursor_input());
        Input::handle_mouse_button_input(m_scene->registry(), m_gameWindow->mouse_button_input());
        m_gameWindow->update_catched_mouse(m_scene->registry());
        m_gameWindow->update_descriptor(m_scene->registry());

        // --- Update game state ---
        m_scene->update();

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        post_processing_framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Light::update_lights(m_scene->registry(), standard_material_shader);
        Render::render(m_scene->registry());

        Framebuffer::unbind();
        post_processing_framebuffer.draw(post_processing_shader);

        glfwSwapBuffers(&m_gameWindow->glfw_window());

        // Update window size
        if (m_gameWindow->dimensions_changed()) {
            auto dimensions = m_gameWindow->physical_dimensions();
            post_processing_framebuffer = Framebuffer(dimensions);
        }
    }
}
