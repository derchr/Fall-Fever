#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#ifdef _DEBUG
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include "Controller.h"
#include "Entity.h"
#include "Helper.h"
#include "JsonParser.h"
#include "Model.h"
#include "Screen.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Widget.h"
#include "World.h"
#include "Menu.h"
#include "Camera.h"
#include "Light.h"
#include "Window.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "EventHandler.h"

Controller::Controller()
{
    m_gameWindow = new Window();
    m_gameEventHandler = new EventHandler(m_gameWindow->getGLFWwindow());

    m_camera = new Camera(90.0f, m_gameWindow->getWindowAspectRatio());

    JsonParser shaderParser("data/shaderPrograms.json");
    m_shaderPrograms = shaderParser.getShaderPrograms();

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
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_gameWindow->getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 150");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
#endif
}

Controller::~Controller()
{
#ifdef _DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    for (auto it = m_shaderPrograms.begin(); it != m_shaderPrograms.end(); it++) {
        delete *it;
    }

    delete m_world;
    delete m_camera;
    delete m_menu;
    delete m_postProcessFrameBuffer;
    delete m_gameEventHandler;
    delete m_gameWindow;
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

    // This is the game loop
    while (!glfwWindowShouldClose(m_gameWindow->getGLFWwindow())) {

        // --- Timing ---
        limit_framerate();

        // --- Update game ---

        static bool rotateLightSource = false, rotateEntity = false;
        if (rotateLightSource) {
            float radius = 4.0;
            glm::vec3 newPos = glm::vec3(-cos(glfwGetTime() * 0.5), 0.5f, sin(glfwGetTime() * 0.5)) * radius;
            m_world->getEntityByName("light")->setPosition(newPos);
        }
        if (rotateEntity) {
            m_world->getEntityById(0)->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.2f * m_deltaTime);
        }
        static glm::vec3 lightColor = glm::vec3(1.f);
        static float intensity = 7.5f;
        m_world->updatePointLight(0, true, m_world->getEntityByName("light")->getPosition(), lightColor, intensity);
        m_world->updateDirectionalLight(true, m_world->getDirectionalLight()->getDirection(), lightColor);
        getShaderProgramByName("lightProgram")->bind();
        getShaderProgramByName("lightProgram")->setUniform("v_lightColor", lightColor * 100.0f);
        getShaderProgramByName("lightProgram")->unbind();

        // --- Render and buffer swap ---

        // Calc shadows
        static bool drawShadows = false;
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
            renderImGui(m_world, &lightColor, &rotateEntity, &rotateLightSource,
                        getShaderProgramByName("postProcessingProgram"), &intensity, &drawShadows);
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
            m_menu->handleMouseButtonActionMap(m_gameEventHandler->getMouseButtonActionMap(), m_gameWindow);
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
    for (auto it = m_shaderPrograms.begin(); it != m_shaderPrograms.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] ShaderProgram could not be found by name \"" << name << "\"" << std::endl;
    return nullptr;
}

ShaderProgram *Controller::getShaderProgramByName(std::vector<ShaderProgram *> shaderPrograms, const std::string &name)
{
    for (auto it = shaderPrograms.begin(); it != shaderPrograms.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] ShaderProgram could not be found by name \"" << name << "\"" << std::endl;
    return nullptr;
}

void Controller::setMaxFps(uint16_t fps)
{
    m_MAX_FPS = fps;
}

#ifdef _DEBUG
void Controller::renderImGui(World *world, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource,
                             ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // render your GUI
    ImGui::Begin("Debug Utils");
    ImGui::Text("Object");
    static float rotation = 0.0;
    ImGui::SliderFloat("Rotation", &rotation, 0, 2 * M_PI);
    static float translation[] = {0.0f, 1.0f, 0.0f};
    ImGui::SliderFloat3("Position", translation, -4.0, 4.0);
    static float scale = 0.6f;
    ImGui::SliderFloat("Scale", &scale, 0.02, 2.0);
    ImGui::Checkbox("Rotate Object", rotateEntity);

    Entity *mainObject = world->getEntityById(0);
    mainObject->setPosition(glm::vec3(translation[0], translation[1], translation[2]));
    if (!*rotateEntity) {
        mainObject->setRotation(glm::vec3(0.f, 1.0f, 0.f), rotation);
    }
    mainObject->setScale(scale);

    // color picker
    ImGui::Text("\nLight Source");

    updateExposure(postProcessingProgram);

    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    ImGui::SliderFloat("Intensity", intensity, 0, 250.f);

    ImGui::ColorEdit3("Color", color);
    lightColor->x = color[0];
    lightColor->y = color[1];
    lightColor->z = color[2];

    ImGui::Text("\nMiscellaneous");
    ImGui::SliderFloat("Exposure", &m_exposure, 0, 5.0f);

    ImGui::Checkbox("Draw Shadows", drawShadows);
    ImGui::Checkbox("Rotate Lightsource", rotateLightSource);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif
