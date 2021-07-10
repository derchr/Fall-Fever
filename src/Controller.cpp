#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#ifdef _DEBUG
#include "imgui/GeneralInfoWindow.h"
#include "imgui/Handler.h"
#endif

#include "Camera.h"
#include "Controller.h"
#include "Entity.h"
#include "EventHandler.h"
#include "Helper.h"
#include "JsonParser.h"
#include "Light.h"
#include "Menu.h"
#include "Model.h"
#include "Screen.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Widget.h"
#include "Window.h"
#include "World.h"

Controller::Controller() : m_gameWindow(std::unique_ptr<Window>(new Window))
{
    m_gameEventHandler = new EventHandler(m_gameWindow->getGLFWwindow());

    m_camera = new Camera(90.0f, m_gameWindow->getWindowAspectRatio());

    JsonParser shaderParser("data/shaderPrograms.json");
    auto shaderProgramPrototypes = shaderParser.getShaderProgramPrototypes();

    for (auto &prototype : shaderProgramPrototypes) {
        m_shaderPrograms.push_back(new ShaderProgram(prototype));
    }

    m_postProcessFrameBuffer = new FrameBuffer(m_gameWindow->getWindowWidth(), m_gameWindow->getWindowHeight(),
                                               getShaderProgramByName("postProcessingProgram"));

    m_menu = new Menu(m_postProcessFrameBuffer, getShaderProgramByName("menuProgram"));

    // Show loading screen...
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_menu->showScreenByName("loadingScreen");
    glfwSwapBuffers(m_gameWindow->getGLFWwindow());

    // Show main menu when loading is finished...
    m_menu->showScreenByName("mainMenuScreen");

    m_world = new World(m_shaderPrograms);

#ifdef _DEBUG
    m_imguiHandler = std::unique_ptr<Imgui::Handler>(new Imgui::Handler(m_gameWindow->getGLFWwindow()));
#endif
}

Controller::~Controller()
{
    for (auto program : m_shaderPrograms) {
        delete program;
    }

    delete m_world;
    delete m_camera;
    delete m_menu;
    delete m_postProcessFrameBuffer;
    delete m_gameEventHandler;
}

Controller &Controller::instance()
{
    static Controller s_instance;
    return s_instance;
}

void Controller::run()
{
    updateExposure(getShaderProgramByName("postProcessingProgram"));

    Entity *lightSource = m_world->getEntityByName("light");
    lightSource->setScale(0.1f);
    lightSource->setRotation(glm::vec3(0.f));
    lightSource->setPosition(glm::vec3(-2.f, 1.5f, 2.f));
    lightSource->setIsLightSource(true);

    m_camera->translate(glm::vec3(0.0f, 1.5f, 5.0f));

    // imgui stuff
    bool rotateLightSource = false, rotateEntity = false, drawShadows = false;
    glm::vec3 lightColor = glm::vec3(1.f);
    float intensity = 7.5f;
#ifdef _DEBUG
    std::shared_ptr<Imgui::Window> imguiWindow = std::make_shared<Imgui::GeneralInfoWindow>(
        this, m_world, getShaderProgramByName("postProcessingProgram"), &rotateEntity, &drawShadows, &rotateLightSource,
        &lightColor, &m_exposure, &intensity);
    m_imguiHandler->addImguiWindow(imguiWindow);
#endif

    // This is the game loop
    while (!glfwWindowShouldClose(m_gameWindow->getGLFWwindow())) {

        // --- Timing ---
        limit_framerate();

        // --- Update game ---

        if (rotateLightSource) {
            float radius = 4.0;
            glm::vec3 newPos = glm::vec3(-cos(glfwGetTime() * 0.5), 0.5f, sin(glfwGetTime() * 0.5)) * radius;
            m_world->getEntityByName("light")->setPosition(newPos);
        }
        if (rotateEntity) {
            m_world->getEntityById(0)->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.2f * m_deltaTime);
        }

        m_world->updatePointLight(0, true, m_world->getEntityByName("light")->getPosition(), lightColor, intensity);
        m_world->updateDirectionalLight(true, m_world->getDirectionalLight()->getDirection(), lightColor);
        getShaderProgramByName("lightProgram")->bind();
        getShaderProgramByName("lightProgram")->setUniform("v_lightColor", lightColor * 100.0f);
        getShaderProgramByName("lightProgram")->unbind();

        // --- Render and buffer swap ---

        // Calc shadows
        static bool firstRun = true;
        getShaderProgramByName("defaultProgram")->bind();
        getShaderProgramByName("defaultProgram")->setUniform("b_drawShadows", (int)drawShadows);
        getShaderProgramByName("defaultProgram")->unbind();
        if (drawShadows || firstRun) {
            firstRun = false;
            m_world->calculateShadows(getShaderProgramByName("directionalShadowDepthProgram"),
                                      getShaderProgramByName("pointShadowDepthProgram"));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto activeScreen = m_menu->getActiveScreen();
        if (activeScreen) {
            activeScreen->draw();
        } else {
            m_postProcessFrameBuffer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_camera->lookForward();
            m_camera->updateVPM();

            m_world->getSkybox()->draw(m_camera->getView(), m_camera->getProj());
            m_world->draw(m_camera->getViewProj(), m_camera->getPosition());

            m_postProcessFrameBuffer->unbind();
            m_postProcessFrameBuffer->render();

#ifdef _DEBUG
            m_imguiHandler->renderWindows();
#endif
        }
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

        m_menu->writeWindowActions(m_gameEventHandler->getWindowActionMap());
        m_gameWindow->handleWindowActionMap(m_gameEventHandler->getWindowActionMap());

        // Handle widget pressed event only when a screen is currently active
        if (m_menu->getActiveScreen())
            m_menu->handleMouseButtonActionMap(m_gameEventHandler->getMouseButtonActionMap(), m_gameWindow.get());
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

    std::cout << "[Warning] ShaderProgram could not be found by name \"" << name << "\"" << std::endl;
    return nullptr;
}

void Controller::setMaxFps(uint16_t fps)
{
    m_MAX_FPS = fps;
}
