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
#include "World.h"

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
    delete pp_framebuffer;
    glfwTerminate();

}

void Controller::run() {
    glClearColor(0.0015f, 0.0015f, 0.0015f, 1.0f);

    ShaderProgram shaderProgram("res/shaders/basic.vert", "res/shaders/basic.frag");
    ShaderProgram lightProgram("res/shaders/light.vert", "res/shaders/light.frag");
    ShaderProgram skyboxProgram("res/shaders/skybox.vert", "res/shaders/skybox.frag");
    ShaderProgram postProcessingProgram("res/shaders/postprocessing.vert", "res/shaders/postprocessing.frag");

    updateExposure(&postProcessingProgram);

    Model model_backpack("res/models/backpack.ffo");
    //Model model_plant("res/models/plant.ffo");
    //Model model_container("res/models/container.ffo");
    Model model_cube("res/models/cube.ffo");
    Model model_dragon("res/models/dragon.ffo");
    Model model_ground("res/models/wood_floor.ffo");
    //Model model_moon("res/models/moon.ffo");
    //Model model_hut("res/models/hut.ffo");
    //Model model_sphere("res/models/sphere.ffo");

    //Entity backpack(&model_backpack, &shaderProgram);
    //Entity sphere(&model_sphere, &shaderProgram);
    //Entity container(&model_container, &shaderProgram);
    //Entity hut(&model_hut, &shaderProgram);
    //Entity moon(&model_moon, &shaderProgram);
    //Entity plant(&model_plant, &shaderProgram);
    Entity dragon(&model_backpack, &shaderProgram);
    Entity ground(&model_ground, &shaderProgram);
    Entity lightSource(&model_cube, &lightProgram);

    dragon.setScale(0.2f);
    lightSource.setScale(0.1f);
    lightSource.setRotation(glm::vec3(0.f));
    lightSource.setPosition(glm::vec3(-2.f, 1.5f, 2.f));

    Skybox skybox(&model_cube, &skyboxProgram, "res/textures/skybox/");

    World world(&shaderProgram);
    world.addEntity(dragon);
    world.addEntity(lightSource);
    world.addEntity(ground);
    
    camera->translate(glm::vec3(0.0f, 1.5f, 5.0f));

    pp_framebuffer = new Framebuffer(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, &postProcessingProgram);

    // This is the game loop
    while(!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {
        // Timing
        limit_framerate();

        // Update game
        // ...
        static bool rotateLightSource = 0;
        if(rotateLightSource) {
            float radius = 4.0;
            glm::vec3 newPos = glm::vec3(-cos(glfwGetTime()*0.5), 0.5f, sin(glfwGetTime()*0.5)) * radius;
            world.getEntities()->operator[](1).setPosition(newPos);
        }
        static glm::vec3 lightColor = glm::vec3(1.f); static float intensity = 1.0f;
        world.updatePointLight(0, true, world.getEntities()->operator[](1).getPosition(), lightColor * intensity);
        world.updateDirectionalLight(true, glm::vec3(-0.2f, -1.0f, -0.3f), lightColor * 0.1f);
        lightProgram.bind();
        lightProgram.setUniform("v_lightColor", lightColor * 100.0f);
        lightProgram.unbind();

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        pp_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        camera->lookForward();
        camera->updateVPM();

        // Calc shadows
        // ...

        glViewport(0, 0, gameWindow->getWindowWidth(), gameWindow->getWindowHeight());
        skybox.draw(camera->getView(), camera->getProj());
        world.draw(camera->getViewProj(), camera->getPosition());
        pp_framebuffer->unbind();

        pp_framebuffer->render();

        #ifdef _DEBUG
        renderImGui(world.getEntities(), &world.getPointLights()[0], &lightColor, &rotateLightSource, &postProcessingProgram, &intensity);
        #endif

        glfwSwapBuffers(gameWindow->getGLFWwindow());
            
        // Update window size
        if(gameWindow->checkWindowWasResized())
            updateWindowSize(&postProcessingProgram);

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

void Controller::updateWindowSize(ShaderProgram *pp_program) {
    camera->updateAspectRatio(gameWindow->getWindowAspectRatio());
    gameEventHandler->setFirstMouseInput(1);

    delete pp_framebuffer;
    pp_framebuffer = new Framebuffer(gameWindow->getWindowWidth(), gameWindow->getWindowHeight(), pp_program);
}

void Controller::updateExposure(ShaderProgram *shaderProgram) {
    shaderProgram->bind();
    shaderProgram->setUniform("u_exposure", exposure);
    shaderProgram->unbind();
}

#ifdef _DEBUG
void Controller::renderImGui(std::vector<Entity> *entites, PointLight *pointLight, glm::vec3 *lightColor, bool *rotateLightSource, ShaderProgram *postProcessingProgram, float *intensity) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // render your GUI
    ImGui::Begin("Debug Utils");
    ImGui::Text("Object");
    static float rotation = 0.0;
    ImGui::SliderFloat("Rotation", &rotation, 0, 2 * M_PI);
    static float translation[] = {0.0f, 0.0f, 0.0f};
    ImGui::SliderFloat3("Position", translation, -4.0, 4.0);
    static float scale = 0.2f;
    ImGui::SliderFloat("Scale", &scale, 0.02, 2.0);

    entites->operator[](0).setPosition(glm::vec3(translation[0], translation[1], translation[2]));
    entites->operator[](0).setRotation(glm::vec3(0.f,1.0f,0.f), rotation);
    entites->operator[](0).setScale(scale);

    // color picker
    ImGui::Text("\nLight Source");
    static float K_q = 0.062f;
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

    ImGui::Checkbox("Rotate Lightsource", rotateLightSource);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
} 
#endif
