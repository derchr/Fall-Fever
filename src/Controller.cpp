#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef _DEBUG
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include "helper.h"
#include "Controller.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Model.h"
#include "Entity.h"
#include "World.h"
#include "Widget.h"
#include "Screen.h"
#include "JsonParser.h"

Controller::Controller()
{
    gameWindow = new Window();
    gameEventHandler = new EventHandler(gameWindow->getGLFWwindow());

    camera = new Camera(90.0f, gameWindow->getWindowAspectRatio());

    JsonParser shaderParser("res/shaderPrograms.json");
    shaderPrograms = shaderParser.getShaderPrograms();

    pp_framebuffer = new Framebuffer(gameWindow->getWindowWidth(), gameWindow->getWindowHeight(), getShaderProgramByName("postProcessingProgram"));

    menu = new Menu(pp_framebuffer, getShaderProgramByName("menuProgram"));

    // Show loading screen...
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    menu->showLoadingScreen();
    glfwSwapBuffers(gameWindow->getGLFWwindow());

    world = new World(shaderPrograms);

#ifdef _DEBUG
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(gameWindow->getGLFWwindow(), true);
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

    for (auto it = shaderPrograms.begin(); it != shaderPrograms.end(); it++) {
        delete *it;
    }

    delete world;
    delete camera;
    delete menu;
    delete pp_framebuffer;
    delete gameEventHandler;
    delete gameWindow;
}

void Controller::run()
{
    updateExposure(getShaderProgramByName("postProcessingProgram"));

    Entity *lightSource = world->getEntityByName("light");
    lightSource->setScale(0.1f);
    lightSource->setRotation(glm::vec3(0.f));
    lightSource->setPosition(glm::vec3(-2.f, 1.5f, 2.f));
    lightSource->setIsLightSource(true);

    camera->translate(glm::vec3(0.0f, 1.5f, 5.0f));

    // This is the game loop
    while (!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {

        // --- Timing ---
        limit_framerate();

        // --- Update game ---

        static bool rotateLightSource = false, rotateEntity = false;
        if (rotateLightSource) {
            float radius = 4.0;
            glm::vec3 newPos = glm::vec3(-cos(glfwGetTime() * 0.5), 0.5f, sin(glfwGetTime() * 0.5)) * radius;
            world->getEntityByName("light")->setPosition(newPos);
        }
        if (rotateEntity) {
            world->getEntityById(0)->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.2f * deltaTime);
        }
        static glm::vec3 lightColor = glm::vec3(1.f);
        static float intensity = 20.f;
        world->updatePointLight(0, true, world->getEntityByName("light")->getPosition(), lightColor, intensity);
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
            world->calculateShadows(getShaderProgramByName("directionalShadowDepthProgram"), getShaderProgramByName("pointShadowDepthProgram"));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pp_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera->lookForward();
        camera->updateVPM();

        glViewport(0, 0, gameWindow->getWindowWidth(), gameWindow->getWindowHeight());

        world->getSkybox()->draw(camera->getView(), camera->getProj());
        world->draw(camera->getViewProj(), camera->getPosition());

        pp_framebuffer->unbind();
        pp_framebuffer->render();

#ifdef _DEBUG
        renderImGui(world, world->getPointLights()[0], &lightColor, &rotateEntity, &rotateLightSource, getShaderProgramByName("postProcessingProgram"), &intensity, &drawShadows);
#endif

        glfwSwapBuffers(gameWindow->getGLFWwindow());

        // Update window size
        if (gameWindow->isWindowResized()) {
            gameWindow->updateWindowDimensions();
            updateWindowDimensions();
        }

        // --- Check events, handle input ---
        gameEventHandler->handleEvents();

        camera->updatePositionFromKeyboardInput(gameEventHandler->getCameraActionRegister(), deltaTime);
        if (gameWindow->getMouseIsCatched()) {
            camera->updateDirectionFromMouseInput(gameEventHandler->getCursorDelta());
        }
        
        gameWindow->handleActionRegister(gameEventHandler->getWindowActionRegister());
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

    deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

void Controller::updateWindowDimensions()
{
    camera->updateAspectRatio(gameWindow->getWindowAspectRatio());
    gameEventHandler->setFirstMouseInput(1);

    pp_framebuffer->changeDimensions(gameWindow->getWindowWidth(), gameWindow->getWindowHeight());
}

void Controller::updateExposure(ShaderProgram *shaderProgram)
{
    shaderProgram->bind();
    shaderProgram->setUniform("u_exposure", exposure);
    shaderProgram->unbind();
}

ShaderProgram* Controller::getShaderProgramByName(const char *name)
{
    for (auto it = shaderPrograms.begin(); it != shaderPrograms.end(); it++) {
        if((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] ShaderProgram could not be found by name \"" << name << "\"" << std::endl;
    return nullptr;
}

ShaderProgram* Controller::getShaderProgramByName(std::vector<ShaderProgram*> shaderPrograms, const char *name)
{
    for (auto it = shaderPrograms.begin(); it != shaderPrograms.end(); it++) {
        if((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] ShaderProgram could not be found by name \"" << name << "\"" << std::endl;
    return nullptr;
}

void Controller::setMaxFps(uint16_t fps)
{
    MAX_FPS = fps;
}


#ifdef _DEBUG
void Controller::renderImGui(World *world, PointLight *pointLight, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource, ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows)
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
    static float K_q = 1.0f;
    ImGui::SliderFloat("Attenuation Parameter", &K_q, 0, 1.5f);

    updateExposure(postProcessingProgram);
    pointLight->setParameters(K_q);

    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    ImGui::SliderFloat("Intensity", intensity, 0, 50.f);

    ImGui::ColorEdit3("Color", color);
    lightColor->x = color[0];
    lightColor->y = color[1];
    lightColor->z = color[2];

    ImGui::Text("\nMiscellaneous");
    ImGui::SliderFloat("Exposure", &exposure, 0, 5.0f);

    ImGui::Checkbox("Draw Shadows", drawShadows);
    ImGui::Checkbox("Rotate Lightsource", rotateLightSource);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif
