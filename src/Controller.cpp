#include "Controller.h"
#include "FrameBuffer.h"
#include "Helper.h"
#include "Light.h"
#include "Window.h"
#include "gltf_loader.h"
#include "render.h"
#include "shader.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <filesystem>
#include <fx/gltf.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <utility>

using namespace entt::literals;

Controller::Controller()
    : m_gameWindow(std::make_shared<Window>()),
      m_postProcessFrameBuffer(m_gameWindow->dimensions().first,
                               m_gameWindow->dimensions().second,
                               post_processing_shader),
      m_gltf_loader{.image_cache = m_image_cache,
                    .material_cache = m_material_cache,
                    .mesh_cache = m_mesh_cache,
                    .shader_cache = m_shader_cache,
                    .scene_cache = m_scene_cache,
                    .gltf_mesh_cache = m_gltf_mesh_cache,
                    .gltf_node_cache = m_gltf_node_cache},
      m_gltf_cache(m_gltf_loader)
{
    // std::filesystem::path document_path("Lantern/glTF-Binary/Lantern.glb");
    std::filesystem::path document_path("ABeautifulGame.glb");
    entt::hashed_string document_hash(document_path.c_str());

    entt::resource<Gltf> gltf_document =
        m_gltf_cache.load(document_hash, document_path).first->second;

    m_scene = gltf_document->default_scene.value().handle();
}

void Controller::run()
{
    updateExposure(post_processing_shader);

    entt::hashed_string shader_hash(Material::SHADER_NAME.data());
    auto standard_material_shader =
        m_shader_cache.load(shader_hash, Material::SHADER_NAME).first->second;

    DirectionalLight directional_light(
        DirectionalLight::Prototype("", glm::vec3(-0.2, -1.0, -0.3), glm::vec3(1.0f), 5.f),
        standard_material_shader.handle().get());
    directional_light.setActive(true);

    PointLight point_light(
        PointLight::Prototype("", "", glm::vec3(4.0, 1.0, 6.0), glm::vec3(1.0F), 3.0F),
        standard_material_shader.handle().get());
    point_light.setActive(true);

    Log::logger().info("Startup complete. Enter game loop.");

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->glfw_window()) == GLFW_FALSE) {
        // --- Timing ---
        limit_framerate();

        // --- Check events, handle input ---
        m_gameWindow->clear_mouse_cursor_input();
        glfwPollEvents();

        auto const &key_input = m_gameWindow->key_input();
        auto const &mouse_cursor_input = m_gameWindow->mouse_cursor_input();

        static constexpr auto MICROSECONDS_PER_SECOND = 1'000'000;
        m_scene->update(
            std::chrono::microseconds(static_cast<unsigned>(m_deltaTime * MICROSECONDS_PER_SECOND)),
            key_input,
            mouse_cursor_input,
            m_gameWindow->aspectRatio(),
            m_gameWindow->cursor_catched());

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_postProcessFrameBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Render::render(m_scene->registry());

        Framebuffer::unbind();
        m_postProcessFrameBuffer.drawOnEntireScreen();

        glfwSwapBuffers(&m_gameWindow->glfw_window());

        // Update window size
        if (m_gameWindow->dimensions_changed()) {
            m_gameWindow->update_dimensions();
            update_window_dimensions();
        }
    }
}

void Controller::limit_framerate()
{
    static double startingTime = 0.0;
    static double lastTime = 0.0;

    lastTime = glfwGetTime() - startingTime;

    double frameTime = 1 / (double)MAX_FPS;
    if (frameTime > lastTime) {
        static constexpr auto MICROSECONDS_PER_SECOND = 1'000'000;
        auto sleep_time_us =
            static_cast<unsigned>((frameTime - lastTime) * MICROSECONDS_PER_SECOND);
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_time_us));
    }

    m_deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

void Controller::update_window_dimensions()
{
    // m_camera->updateAspectRatio(m_gameWindow->aspectRatio());
    // m_gameEventHandler->setFirstMouseInput(1);

    auto dimensions = m_gameWindow->dimensions();
    m_postProcessFrameBuffer.changeDimensions(dimensions.first, dimensions.second);
}

void Controller::updateExposure(Shader &shader) const
{
    shader.bind();
    shader.set_uniform("u_exposure", m_exposure);
    Shader::unbind();
}
