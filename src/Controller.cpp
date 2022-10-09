#include "Controller.h"
#include "Camera.h"
#include "Entity.h"
#include "FrameBuffer.h"
#include "Helper.h"
#include "Light.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <array>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Controller::Controller()
    : m_gameWindow(std::make_shared<Window>()),
      m_camera(std::make_shared<Camera>(90., m_gameWindow->aspectRatio())),
      m_postProcessFrameBuffer(m_gameWindow->dimensions().first, m_gameWindow->dimensions().second,
                               postProcessingProgram)
{
}

void Controller::run()
{
    updateExposure(postProcessingProgram);

    m_camera->translate(glm::vec3(0., 1.5, 5.));

    Log::logger().info("Startup complete.");

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->glfw_window()) == GLFW_FALSE) {

        // --- Timing ---
        limit_framerate();

        // --- Update game ---
        lightProgram.bind();
        lightProgram.setUniform("v_lightColor", glm::vec3{1., 1., 1.} * 100.0F);
        lightProgram.unbind();

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_postProcessFrameBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_camera->lookForward();
        m_camera->updateVPM();

        m_postProcessFrameBuffer.unbind();
        m_postProcessFrameBuffer.drawOnEntireScreen();

        glfwSwapBuffers(&m_gameWindow->glfw_window());

        // Update window size
        if (m_gameWindow->dimensions_changed()) {
            m_gameWindow->update_dimensions();
            update_window_dimensions();
        }

        // --- Check events, handle input ---
        m_gameWindow->clear_mouse_cursor_input();
        glfwPollEvents();

        auto const &key_input = m_gameWindow->key_input();
        auto const &mouse_cursor_input = m_gameWindow->mouse_cursor_input();
        // auto const &mouse_button_input = m_gameWindow->mouse_button_input();

        m_camera->updatePositionFromKeyboardInput(key_input, (float)m_deltaTime);

        if (m_gameWindow->cursor_catched()) {
            m_camera->updateDirectionFromMouseInput(mouse_cursor_input);
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
        Helper::sleep(static_cast<unsigned>(frameTime - lastTime) * 1000000);
    }

    m_deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

void Controller::update_window_dimensions()
{
    m_camera->updateAspectRatio(m_gameWindow->aspectRatio());
    // m_gameEventHandler->setFirstMouseInput(1);

    auto dimensions = m_gameWindow->dimensions();
    m_postProcessFrameBuffer.changeDimensions(dimensions.first, dimensions.second);
}

void Controller::updateExposure(ShaderProgram &shaderProgram) const
{
    shaderProgram.bind();
    shaderProgram.setUniform("u_exposure", m_exposure);
    shaderProgram.unbind();
}
