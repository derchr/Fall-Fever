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

    pp_framebuffer = new Framebuffer(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);

    // This is the game loop
    while(!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {
        // Timing
        limit_framerate();

        // Update game
        // ...
        static bool rotateLightSource = 0;
        if(rotateLightSource) {
            float radius = 25.0;
            glm::vec3 newPos = glm::vec3(-cos(glfwGetTime()*0.5), 0.5f, sin(glfwGetTime()*0.5)) * radius;
            world.getEntities()->operator[](1).setPosition(newPos);
        }
        static glm::vec3 lightColor = glm::vec3(1.f);
        world.updatePointLight(0, true, world.getEntities()->operator[](1).getPosition(), lightColor);
        world.updateDirectionalLight(true, glm::vec3(-0.2f, -1.0f, -0.3f), lightColor * 0.001f);
        lightProgram.bind();
        lightProgram.setUniform("v_lightColor", lightColor);
        lightProgram.unbind();

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        pp_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        camera->lookForward();
        camera->updateVPM();

        skybox.draw(camera->getView(), camera->getProj());
        world.draw(camera->getViewProj(), camera->getPosition());
        pp_framebuffer->unbind();

        postProcessingProgram.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pp_framebuffer->getTextureId());
        GLint location = glGetUniformLocation(postProcessingProgram.getShaderProgramId(), "u_texture");
        glUniform1i(location, 0);
        // Disable wireframe mode
        GLint wireframe;
        glGetIntegerv(GL_POLYGON_MODE, &wireframe);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        GLuint temp_vao;
        glGenVertexArrays(1, &temp_vao);
        glBindVertexArray(temp_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe);
        postProcessingProgram.unbind();

        #ifdef _DEBUG
        renderImGui(world.getEntities(), &world.getPointLights()[0], &lightColor, &rotateLightSource);
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

    delete pp_framebuffer;
    pp_framebuffer = new Framebuffer(gameWindow->getWindowWidth(), gameWindow->getWindowHeight());
}

#ifdef _DEBUG
void Controller::renderImGui(std::vector<Entity> *entites, PointLight *pointLight, glm::vec3 *lightColor, bool *rotateLightSource) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // render your GUI
    ImGui::Begin("Debug Utils");
    ImGui::Text("Dragon");
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
    ImGui::Text("Attenuation");
    static float K_c = 1.0f, K_l = 0.09f, K_q = 0.062f;
    ImGui::SliderFloat("Constant Term", &K_c, 0, 1.0f*5);
    ImGui::SliderFloat("Linear Term", &K_l, 0, 0.09f*5);
    ImGui::SliderFloat("Quadratic Term", &K_q, 0, 0.032f*10);

    pointLight->setParameters(K_c, K_l, K_q);

    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ImGui::Text("Color");
    ImGui::ColorEdit3("Color", color);
    lightColor->x = color[0];
    lightColor->y = color[1];
    lightColor->z = color[2];

    ImGui::Checkbox("Rotate Lightsource", rotateLightSource);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
} 
#endif
