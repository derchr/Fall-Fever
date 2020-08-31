#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>

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
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    shaderProgram = new ShaderProgram("res/shaders/basic.vs", "res/shaders/basic.fs");
    shaderProgram->bind();

    uint32_t numVertices = sizeof(vertices) / sizeof(Vertex);

    vertexBuffer = new VertexBuffer(vertices, numVertices);

    // This is the game loop
    while(!glfwWindowShouldClose(gameWindow->getGLFWwindow())) {
        // Timing
        limit_framerate();
        static int frameCount = 55;
        if(frameCount++ == 60) {
            std::cout << "FPS: " << 1/deltaTime << std::endl;
            frameCount = 0;
        }

        // Update game
        // ...

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT);

        vertexBuffer->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vertexBuffer->unbind();

        glfwSwapBuffers(gameWindow->getGLFWwindow());

        // Check events, handle input
        gameEventHandler->handleEvents(gameWindow->getGLFWwindow());
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
