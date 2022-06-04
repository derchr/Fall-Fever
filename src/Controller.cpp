#include "Controller.h"
#include "Camera.h"
#include "Entity.h"
#include "EventHandler.h"
#include "Helper.h"
#include "Light.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Window.h"
#include "resources/Model.h"
#include "resources/ResourceHandler.h"
#include "resources/Texture.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Controller::Controller() : m_gameWindow(std::unique_ptr<Window>(new Window))
{
    m_gameEventHandler = new EventHandler(m_gameWindow->getGLFWwindow());

    m_camera = new Camera(90.0f, m_gameWindow->getWindowAspectRatio());

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
        m_shaderPrograms.push_back(new ShaderProgram(prototype));
        Log::logger().info("Loaded shaderprogram \"{}\"", prototype.name);
    }

    m_postProcessFrameBuffer = new FrameBuffer(m_gameWindow->getWindowWidth(), m_gameWindow->getWindowHeight(),
                                               getShaderProgramByName("postProcessingProgram"));

    m_scene = new Scene(m_shaderPrograms);
}

Controller::~Controller()
{
    for (auto program : m_shaderPrograms) {
        delete program;
    }

    delete m_scene;
    delete m_camera;
    delete m_postProcessFrameBuffer;
    delete m_gameEventHandler;
}

void Controller::run()
{
    updateExposure(getShaderProgramByName("postProcessingProgram"));

    ModelEntity *lightSource = m_scene->getEntityByName("light");
    lightSource->setScale(0.1f);
    lightSource->setRotation(glm::vec3(0.f));
    lightSource->setPosition(glm::vec3(-2.f, 1.5f, 2.f));

    m_camera->translate(glm::vec3(0.0f, 1.5f, 5.0f));

    bool drawShadows = false;
    float intensity = 7.5;
    glm::vec3 lightColor{1., 1., 1.};

    // This is the game loop
    while (!glfwWindowShouldClose(m_gameWindow->getGLFWwindow())) {

        // --- Timing ---
        limit_framerate();

        // --- Update game ---
        m_scene->updatePointLight(0, true, m_scene->getEntityByName("light")->getPosition(), lightColor, intensity);
        m_scene->updateDirectionalLight(true, m_scene->getDirectionalLight()->getDirection(), lightColor);
        getShaderProgramByName("lightProgram")->bind();
        getShaderProgramByName("lightProgram")->setUniform("v_lightColor", glm::vec3{1., 1., 1.} * 100.0f);
        getShaderProgramByName("lightProgram")->unbind();

        // --- Render and buffer swap ---

        // Calc shadows
        static bool firstRun = true;
        getShaderProgramByName("defaultProgram")->bind();
        getShaderProgramByName("defaultProgram")->setUniform("b_drawShadows", (int)drawShadows);
        getShaderProgramByName("defaultProgram")->unbind();
        if (drawShadows || firstRun) {
            firstRun = false;
            m_scene->calculateShadows(getShaderProgramByName("directionalShadowDepthProgram"),
                                      getShaderProgramByName("pointShadowDepthProgram"));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_postProcessFrameBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_camera->lookForward();
        m_camera->updateVPM();

        m_scene->getSkybox()->draw(m_camera->getView(), m_camera->getProj());
        m_scene->draw(m_camera->getViewProj(), m_camera->getPosition());

        m_postProcessFrameBuffer->unbind();
        m_postProcessFrameBuffer->drawOnEntireScreen();

        glfwSwapBuffers(m_gameWindow->getGLFWwindow());

        // Update window size
        if (m_gameWindow->isWindowResized()) {
            m_gameWindow->updateWindowDimensions();
            updateWindowDimensions();
        }

        // --- Check events, handle input ---
        m_gameEventHandler->handleEvents();

        m_camera->updatePositionFromKeyboardInput(m_gameEventHandler->getCameraActionMap(), m_deltaTime);
        if (m_gameWindow->getMouseIsCatched()) {
            m_camera->updateDirectionFromMouseInput(m_gameEventHandler->getCameraMouseActionMap());
        }

        m_gameWindow->handleWindowActionMap(m_gameEventHandler->getWindowActionMap());
    }
}

void Controller::limit_framerate()
{
    static double startingTime = 0.0;
    static double lastTime = 0.0;

    lastTime = glfwGetTime() - startingTime;

    double frameTime = 1 / (double)m_MAX_FPS;
    if (frameTime > lastTime) {
        Helper::sleep((frameTime - lastTime) * 1000000);
    }

    m_deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

void Controller::updateWindowDimensions()
{
    m_camera->updateAspectRatio(m_gameWindow->getWindowAspectRatio());
    m_gameEventHandler->setFirstMouseInput(1);

    m_postProcessFrameBuffer->changeDimensions(m_gameWindow->getWindowWidth(), m_gameWindow->getWindowHeight());
}

void Controller::updateExposure(ShaderProgram *shaderProgram)
{
    shaderProgram->bind();
    shaderProgram->setUniform("u_exposure", m_exposure);
    shaderProgram->unbind();
}

ShaderProgram *Controller::getShaderProgramByName(const std::string &name)
{
    return getShaderProgramByName(name, m_shaderPrograms);
}

ShaderProgram *Controller::getShaderProgramByName(const std::string &name, std::vector<ShaderProgram *> shaderPrograms)
{
    for (auto program : shaderPrograms) {
        if (program->getUniqueName() == name) {
            return program;
        }
    }
    Log::logger().critical("Shaderprogram could not be found by name \"{}\"", name);
    return nullptr;
}

void Controller::setMaxFps(uint16_t fps)
{
    m_MAX_FPS = fps;
}
