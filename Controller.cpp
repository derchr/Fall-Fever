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
    camera = new Camera(90.0f, gameWindow->getWindowWidth(), gameWindow->getWindowHeight());
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
        Vertex{
            glm::vec3(-0.5f, -0.5f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{
            glm::vec3(0.5f, -0.5f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        Vertex{
            glm::vec3(-0.5f, 0.5f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        Vertex{
            glm::vec3(0.5f, 0.5f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)}
    };

    uint32_t indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    uint32_t numVertices = sizeof(vertices) / sizeof(Vertex);
    uint32_t numIndices = sizeof(indices) / sizeof(indices[0]);

    VertexBuffer vertexBuffer(vertices, indices, numVertices, numIndices);

    Texture tex1("res/tex.png", shaderProgram.getShaderProgramId());

    glm::mat4 model = glm::mat4(1.0f);
    
    camera->translate(glm::vec3(0.0f, 0.0f, 2.5f));

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
        model = glm::rotate(model, (float)(this->deltaTime*0.0005), glm::vec3(0.0f, 1.0f, 0.0f));

        //camera->lookAtTarget(glm::vec3(0.0f, 0.0f, 0.0f));
        camera->lookForward();
        camera->updateVPM();
        glm::mat4 modelViewProj = camera->getViewProj() * model;
        shaderProgram.setUniform("u_modelViewProj", modelViewProj);

        // Render and buffer swap
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vertexBuffer.bind();
        tex1.bind(0);
        for(int i=0;i<20;i++) {
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
            camera->updateVPM();
            glm::mat4 modelViewProj = camera->getViewProj() * model;
            shaderProgram.setUniform("u_modelViewProj", modelViewProj);
        }
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
        tex1.unbind();
        vertexBuffer.unbind();

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
