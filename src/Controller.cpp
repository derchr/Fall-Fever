#include "Controller.h"
#include "Camera.h"
#include "Entity.h"
#include "Helper.h"
#include "Light.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Controller::Controller()
    : m_gameWindow(std::make_shared<Window>()), m_camera(std::make_shared<Camera>(90., m_gameWindow->aspectRatio()))
{
    std::array shaderProgramPrototypes{
        ShaderProgram::Prototype{"defaultProgram", "data/shaders/basic.vert", "data/shaders/basic.frag", ""},
        ShaderProgram::Prototype{"lightProgram", "data/shaders/light.vert", "data/shaders/light.frag", ""},
        ShaderProgram::Prototype{"skyboxProgram", "data/shaders/skybox.vert", "data/shaders/skybox.frag", ""},
        ShaderProgram::Prototype{"postProcessingProgram", "data/shaders/postprocessing.vert",
                                 "data/shaders/postprocessing.frag", ""},
        ShaderProgram::Prototype{"directionalShadowDepthProgram", "data/shaders/directionalShadowDepth.vert",
                                 "data/shaders/directionalShadowDepth.frag", ""},
        ShaderProgram::Prototype{"pointShadowDepthProgram", "data/shaders/pointShadowDepth.vert",
                                 "data/shaders/pointShadowDepth.frag", "data/shaders/pointShadowDepth.geom"},
    };

    for (auto &prototype : shaderProgramPrototypes) {
        m_shaderPrograms.push_back(std::make_shared<ShaderProgram>(prototype));
        Log::logger().info("Loaded shaderprogram \"{}\"", prototype.name);
    }

    auto dimensions = m_gameWindow->dimensions();
    m_postProcessFrameBuffer = std::make_shared<FrameBuffer>(dimensions.first, dimensions.second,
                                                             getShaderProgramByName("postProcessingProgram").get());

    m_scene = std::make_shared<Scene>(m_shaderPrograms);
}

void Controller::run()
{
    updateExposure(*getShaderProgramByName("postProcessingProgram"));

    auto lightSource = m_scene->getEntityByName("light");
    lightSource->setScale(.1);
    lightSource->setRotation(glm::vec3(0.));
    lightSource->setPosition(glm::vec3(-2., 1.5, 2.));

    m_camera->translate(glm::vec3(0., 1.5, 5.));

    static constexpr float INTENSITY = 7.5;
    glm::vec3 lightColor{1., 1., 1.};

    // This is the game loop
    while (glfwWindowShouldClose(&m_gameWindow->glfw_window()) == GLFW_FALSE) {

        // --- Timing ---
        limit_framerate();

        // --- Update game ---
        m_scene->updatePointLight(0, true, m_scene->getEntityByName("light")->getPosition(), lightColor, INTENSITY);
        m_scene->updateDirectionalLight(true, m_scene->getDirectionalLight()->getDirection(), lightColor);
        getShaderProgramByName("lightProgram")->bind();
        getShaderProgramByName("lightProgram")->setUniform("v_lightColor", glm::vec3{1., 1., 1.} * 100.0F);
        getShaderProgramByName("lightProgram")->unbind();

        // --- Render and buffer swap ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_postProcessFrameBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_camera->lookForward();
        m_camera->updateVPM();

        m_scene->getSkybox()->draw(m_camera->getView(), m_camera->getProj());
        m_scene->draw(m_camera->getViewProj(), m_camera->getPosition());

        m_postProcessFrameBuffer->unbind();
        m_postProcessFrameBuffer->drawOnEntireScreen();

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
        Helper::sleep((frameTime - lastTime) * 1000000);
    }

    m_deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

void Controller::update_window_dimensions()
{
    m_camera->updateAspectRatio(m_gameWindow->aspectRatio());
    // m_gameEventHandler->setFirstMouseInput(1);

    auto dimensions = m_gameWindow->dimensions();
    m_postProcessFrameBuffer->changeDimensions(dimensions.first, dimensions.second);
}

void Controller::updateExposure(ShaderProgram &shaderProgram) const
{
    shaderProgram.bind();
    shaderProgram.setUniform("u_exposure", m_exposure);
    shaderProgram.unbind();
}

std::shared_ptr<ShaderProgram> Controller::getShaderProgramByName(const std::string &name)
{
    return getShaderProgramByName(name, m_shaderPrograms);
}

std::shared_ptr<ShaderProgram>
Controller::getShaderProgramByName(const std::string &name, std::vector<std::shared_ptr<ShaderProgram>> shaderPrograms)
{
    for (auto program : shaderPrograms) {
        if (program->getUniqueName() == name) {
            return program;
        }
    }
    Log::logger().critical("Shaderprogram could not be found by name \"{}\"", name);
    return {};
}
