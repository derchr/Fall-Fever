#include <stdlib.h>
#include <iostream>

#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

#include "Controller.h"

Controller::Controller() {
    if(!glfwInit()) exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(error_callback);

    gameWindow = new Window();
    gameEventHandler = new EventHandler();
}

Controller::~Controller() {
    delete gameWindow;
    delete gameEventHandler;
    glfwTerminate();
}

void Controller::run() {
    glClearColor(0.341f, 0.678f, 0.408f, 1.0f);
    // This is the game loop
    while(!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {
        // Timing
        limit_framerate();
        std::cout << "FPS: " << 1/deltaTime << std::endl;

        // Update game

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(gameWindow->getGLFWwindow());


        // Check events, handle input
        gameEventHandler->handleEvents(gameWindow->getGLFWwindow());
    }

}

void Controller::error_callback(int error, const char* description) {
    (void)error;
    fprintf(stderr, "Error: %s\n", description);
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
