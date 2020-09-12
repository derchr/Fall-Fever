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

#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

#include "Controller.h"
#include "Texture.h"
#include "Model.h"
#include "Entity.h"
#include "Scene.h"

Controller::Controller() {
    if(!glfwInit()) exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifndef _DEBUG
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    #endif

    gameWindow = new Window();
    gameEventHandler = new EventHandler(gameWindow->getGLFWwindow());

    camera = new Camera(90.0f, gameWindow->getWindowAspectRatio());

    #ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwSetErrorCallback(error_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(gameWindow->getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 150");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    #endif
}

Controller::~Controller() {

    #ifdef _DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    #endif

    delete gameWindow;
    delete gameEventHandler;
    delete camera;
    glfwTerminate();

}

void Controller::run() {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    ShaderProgram shaderProgram("res/shaders/basic.vert", "res/shaders/basic.frag");
    ShaderProgram lightProgram("res/shaders/light.vert", "res/shaders/light.frag");

    //Model model_backpack("res/models/backpack.ffo");
    //Model model_plant("res/models/plant.ffo");
    Model model_container("res/models/container.ffo");
    Model model_cube("res/models/cube.ffo");
    Model model_dragon("res/models/dragon.ffo");
    //Model model_sphere("res/models/sphere.ffo");

    //Entity backpack(&model_backpack, &shaderProgram);
    //Entity sphere(&model_sphere, &shaderProgram);
    Entity container(&model_container, &shaderProgram);
    Entity dragon(&model_dragon, &shaderProgram);
    //Entity plant(&model_plant, &shaderProgram);
    Entity lightSource(&model_cube, &lightProgram);

    lightSource.translate(glm::vec3(-5.0f, 1.0f, 0.0f));
    lightSource.setScale(0.2f);
    //plant.scale(5.0f);
    dragon.setScale(0.2f);

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);
    glm::vec3 specularColor = glm::vec3(1.0f);

    shaderProgram.bind();
    shaderProgram.setUniform("u_directionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shaderProgram.setUniform("u_directionalLight.ambient", ambientColor * 0.25f);
    shaderProgram.setUniform("u_directionalLight.diffuse", diffuseColor * 0.25f);
    shaderProgram.setUniform("u_directionalLight.specular", specularColor * 0.25f);

    shaderProgram.setUniform("u_material.shininess", 32.0f);
    shaderProgram.unbind();

    Scene scene(&shaderProgram);
    scene.addEntity(dragon);
    scene.addEntity(lightSource);
    
    camera->translate(glm::vec3(0.0f, 0.0f, 7.5f));

    // This is the game loop
    while(!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {
        // Timing
        limit_framerate();

        // Update game
        // ...

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        camera->lookForward();
        camera->updateVPM();

        scene.drawScene(camera->getViewProj(), camera->getPosition());

        #ifdef _DEBUG
        renderImGui(scene.getEntities().data());
        #endif

        glfwSwapBuffers(gameWindow->getGLFWwindow());
            
        // Update window size
        if(gameWindow->checkWindowWasResized())
            updateWindowSize();

        // Check events, handle input
        gameEventHandler->handleEvents();

        camera->updatePositionFromKeyboardInput(gameEventHandler->getCameraActionRegister(), deltaTime);
        if(gameWindow->getMouseIsCatched())
            camera->updateDirectionFromMouseInput(gameEventHandler->getCursorDelta());

        gameWindow->handleActionRegister(gameEventHandler->getWindowActionRegister());
    }

}

void Controller::limit_framerate() {
    static double startingTime = 0.0;
    static double lastTime = 0.0;

    lastTime = glfwGetTime() - startingTime;

    double frameTime = 1/(double)MAX_FPS;
    if(frameTime > lastTime) {
        #ifdef __linux__
        usleep((frameTime - lastTime) * 1000000);
        #endif
        #ifdef _WIN32
        Sleep((frameTime - lastTime) * 1000);
        #endif
    }

    deltaTime = glfwGetTime() - startingTime;

    startingTime = glfwGetTime();
}

// GLFW error function
void Controller::error_callback(int error, const char* description) {
    (void)error;
    fprintf(stderr, "Error: %s\n", description);
}

void Controller::updateWindowSize() {
    camera->updateAspectRatio(gameWindow->getWindowAspectRatio());
    gameEventHandler->setFirstMouseInput(1);
}

#ifdef _DEBUG
void Controller::renderImGui(Entity *entity) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();



    // render your GUI
    ImGui::Begin("Object Modifier");
    static float rotation = 0.0;
    ImGui::SliderFloat("Rotation", &rotation, 0, 2 * M_PI);
    static float translation[] = {0.0f, 0.0f};
    ImGui::SliderFloat2("Position", translation, -4.0, 4.0);
    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    entity->setPosition(glm::vec3(translation[0], 0.0f, translation[1]));
    //entity->setRotation()

    // color picker
    ImGui::ColorEdit3("Color", color);
    ImGui::End();



    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
} 
#endif
