#include "EventHandler.h"

bool EventHandler::wireFrameMode = 0;

EventHandler::EventHandler(GLFWwindow *p_window)
    : window(p_window) {

    glfwSetKeyCallback(window, key_callback);

}

void EventHandler::handleEvents() {
    glfwPollEvents();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void EventHandler::key_callback(GLFWwindow *p_window, int key, int scancode, int action, int mods) {
    // Silence warnings of unused variables.
    (void)p_window; (void)scancode; (void)mods;

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        wireFrameMode = !wireFrameMode;
        if(wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}
