#include <iostream>
#include <vector>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

Controller::Controller() {
    if(!glfwInit()) exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwSetErrorCallback(error_callback);
    #endif

    gameWindow = new Window();
    gameEventHandler = new EventHandler(gameWindow->getGLFWwindow());
    camera = new Camera(90.0f, gameWindow->getWindowWidth(), gameWindow->getWindowHeight());
}

Controller::~Controller() {
    delete gameWindow;
    delete gameEventHandler;
    delete camera;
    glfwTerminate();
}

void Controller::run() {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    ShaderProgram shaderProgram("res/shaders/basic.vs", "res/shaders/basic.fs");

    ShaderProgram lightProgram("res/shaders/light.vs", "res/shaders/light.fs");

    std::vector<Entity> scene;

    Model model_backpack("res/models/backpack.obj");
    Model model_cube("res/models/cube.obj");
    Model model_sphere("res/models/sphere.obj");

    //Entity backpack1(&model_backpack, &shaderProgram);
    Entity cube(&model_backpack, &shaderProgram);
    Entity sphere(&model_sphere, &shaderProgram);
    Entity lightSource(&model_cube, &lightProgram);

    sphere.translate(glm::vec3(5.0f, 0.0f, 2.5f));

    lightSource.translate(glm::vec3(-5.0f, 1.0f, 0.0f));
    lightSource.scale(0.2f);

    shaderProgram.bind();
    shaderProgram.setUniform("u_lightPosition", lightSource.getPosition());
    shaderProgram.setUniform("u_objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    shaderProgram.setUniform("u_lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shaderProgram.unbind();

    scene.push_back(lightSource);
    scene.push_back(cube);
    scene.push_back(sphere);
    
    camera->translate(glm::vec3(0.0f, 0.0f, 7.5f));

    // This is the game loop
    while(!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {
        // Timing
        limit_framerate();

        #ifdef _DEBUG
        static uint8_t frameCount = 250;
        if(frameCount++ == 255) {
            std::cout << "FPS: " << 1/deltaTime << std::endl;
            frameCount = 0;
        }
        #endif

        // Update game
        // ...

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        camera->lookForward();
        camera->updateVPM();

        for(auto it = scene.begin(); it != scene.end(); it++) {
            it->draw(camera->getViewProj(), camera->getPosition());
        }
        //backpack1.draw(camera->getViewProj());
        //cube.draw(camera->getViewProj());

        glfwSwapBuffers(gameWindow->getGLFWwindow());
            
        // Update window size
        if(gameWindow->checkWindowWasResized())
            updateWindowSize();

        // Check events, handle input
        gameEventHandler->handleEvents();

        camera->updatePositionFromKeyboardInput(gameEventHandler->getCameraActionRegister(), deltaTime);
        camera->updateDirectionFromMouseInput(gameEventHandler->getCursorDeltaX(), gameEventHandler->getCursorDeltaY());
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
    camera->updateAspectRatio(gameWindow->getWindowWidth(), gameWindow->getWindowHeight());
    gameEventHandler->setFirstMouseInput(1);
}
