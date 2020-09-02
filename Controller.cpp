#include <stdlib.h>
#include <iostream>
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

Controller::Controller() {
    if(!glfwInit()) exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #endif

    glfwSetErrorCallback(error_callback);

    gameWindow = new Window();
    gameEventHandler = new EventHandler(gameWindow->getGLFWwindow());
}

Controller::~Controller() {
    delete gameWindow;
    delete gameEventHandler;
    glfwTerminate();
}

void Controller::run() {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    ShaderProgram shaderProgram("res/shaders/basic.vs", "res/shaders/basic.fs");
    shaderProgram.bind();

    Vertex vertices[] = {
        Vertex{-0.5f, -0.5f, 0.0f,
            0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,0.0f, 1.0f},
        Vertex{-0.5f, 0.5f, 0.0f,
            0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, 0.5f, 0.0f,
            1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f}
    };

    uint32_t indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    uint32_t numVertices = sizeof(vertices) / sizeof(Vertex);
    uint32_t numIndices = sizeof(indices) / sizeof(indices[0]);

    VertexBuffer vertexBuffer(vertices, indices, numVertices, numIndices);

    Texture tex1("res/tex.png", shaderProgram.getShaderProgramId());

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
        glm::mat4 trans_mat = glm::mat4(1.0f);
        trans_mat = glm::rotate(trans_mat, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        shaderProgram.setUniform("transform", trans_mat);

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT);

        vertexBuffer.bind();
        tex1.bind(0);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        tex1.unbind();
        vertexBuffer.unbind();

        glfwSwapBuffers(gameWindow->getGLFWwindow());

        // Check events, handle input
        gameEventHandler->handleEvents();
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
