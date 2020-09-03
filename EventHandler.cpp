#include "EventHandler.h"

bool EventHandler::wireFrameMode = 0;

EventHandler::EventHandler(GLFWwindow *p_window)
    : window(p_window) {

    glfwSetKeyCallback(window, key_callback);

}

void EventHandler::handleEvents() {
    glfwPollEvents();
    for(int i=0; i<CAMERA_ACTION_NUM_ITEMS; i++) {
        actionCameraRegister[i] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        actionCameraRegister[cameraForward] = 1;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        actionCameraRegister[cameraBackward] = 1;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        actionCameraRegister[cameraUp] = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        actionCameraRegister[cameraDown] = 1;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        actionCameraRegister[cameraLeft] = 1;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        actionCameraRegister[cameraRight] = 1;

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
