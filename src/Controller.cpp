#include "Controller.h"
#include "FrameBuffer.h"
#include "Helper.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "gltf_loader.h"
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
                               postProcessingProgram)
{
    // if (!gltf.cameras.empty()) {
    //     auto const &gltf_camera = gltf.cameras.at(0);

    //     assert(gltf_camera.type == fx::gltf::Camera::Type::Perspective);
    //     auto const &perspective = gltf_camera.perspective;

    //     m_camera = std::make_shared<Camera>(perspective.yfov, perspective.aspectRatio);
    // } else {
    // m_camera = std::make_shared<Camera>(90., m_gameWindow->aspectRatio());
    // }
}

void Controller::run()
{
    updateExposure(postProcessingProgram);

    // m_camera->translate(glm::vec3(0., .5, 2.));

    DirectionalLight directional_light(
        DirectionalLight::Prototype("", glm::vec3(-0.2, -1.0, -0.3), glm::vec3(1.0f), 5.f),
        &defaultProgram);
    directional_light.setActive(true);

    PointLight point_light(
        PointLight::Prototype("", "", glm::vec3(4.0, 1.0, 6.0), glm::vec3(1.0F), 3.0F),
        &defaultProgram);
    point_light.setActive(true);

    Log::logger().info("Startup complete. Enter game loop.");

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->glfw_window()) == GLFW_FALSE) {
        // --- Timing ---
        limit_framerate();

        // --- Update game ---
        m_gameWindow->clear_mouse_cursor_input(); // MOVE DOWN AGAIN!
        glfwPollEvents();

        auto const &key_input = m_gameWindow->key_input();
        auto const &mouse_cursor_input = m_gameWindow->mouse_cursor_input();

        static constexpr auto MICROSECONDS_PER_SECOND = 1'000'000;
        m_scene.update(
            std::chrono::microseconds(static_cast<unsigned>(m_deltaTime * MICROSECONDS_PER_SECOND)),
            key_input,mouse_cursor_input, m_gameWindow->aspectRatio());

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_postProcessFrameBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_scene.draw(&defaultProgram);

        Framebuffer::unbind();
        m_postProcessFrameBuffer.drawOnEntireScreen();

        glfwSwapBuffers(&m_gameWindow->glfw_window());

        // Update window size
        if (m_gameWindow->dimensions_changed()) {
            m_gameWindow->update_dimensions();
            update_window_dimensions();
        }

        // --- Check events, handle input ---
        // m_gameWindow->clear_mouse_cursor_input();
        // glfwPollEvents();

        // auto const &key_input = m_gameWindow->key_input();
        // auto const &mouse_cursor_input = m_gameWindow->mouse_cursor_input();

        // m_camera->updatePositionFromKeyboardInput(key_input, (float)m_deltaTime);

        // if (m_gameWindow->cursor_catched()) {
        //     m_camera->updateDirectionFromMouseInput(mouse_cursor_input);
        // }
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

void Controller::updateExposure(ShaderProgram &shaderProgram) const
{
    shaderProgram.bind();
    shaderProgram.setUniform("u_exposure", m_exposure);
    ShaderProgram::unbind();
}
