#include "EventHandler.h"

void EventHandler::handleEvents(GLFWwindow *window) {
    glfwPollEvents();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

