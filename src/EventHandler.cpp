#include "EventHandler.h"

#include <iostream>

bool EventHandler::cameraActionRegister[CAMERA_ACTION_NUM_ITEMS] = {0};
bool EventHandler::windowActionRegister[WINDOW_ACTION_NUM_ITEMS] = {0};

bool EventHandler::firstMouseInput = 1;


EventHandler::EventHandler(GLFWwindow *window)
    : window(window) {

    glfwSetKeyCallback(window, key_callback);

    // Currently disabled because callbacks are shitty
    //glfwSetCursorPosCallback(window, mouse_callback);

}

void EventHandler::handleEvents() {

    glfwPollEvents();

    for(int i=0; i<CAMERA_ACTION_NUM_ITEMS; i++) {
        cameraActionRegister[i] = 0;
    }

    updateMouseDelta();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraActionRegister[cameraForward] = 1;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraActionRegister[cameraBackward] = 1;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraActionRegister[cameraUp] = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraActionRegister[cameraDown] = 1;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraActionRegister[cameraLeft] = 1;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraActionRegister[cameraRight] = 1;

}

void EventHandler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Silence warnings of unused variables.
    (void)window; (void)scancode; (void)mods; (void)key; (void)action;

    if(key==GLFW_KEY_O && action == GLFW_PRESS)
        windowActionRegister[wireFrameToggle] = 1;
    if(key==GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        windowActionRegister[mouseCatchToggle] = 1;
        firstMouseInput = 1;
    }

}

void EventHandler::updateMouseDelta() {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    deltaCursorPosX = xPos - lastCursorPosX;
    deltaCursorPosY = -(yPos - lastCursorPosY);
    lastCursorPosX = xPos;
    lastCursorPosY = yPos;

    // Check if this is the first VALID mouse event after window being resized
    if(firstMouseInput && !(deltaCursorPosX == 0 && deltaCursorPosY == 0)) {
        firstMouseInput = 0;
        deltaCursorPosX = 0.0f;
        deltaCursorPosY = 0.0f;
    }

    deltaCursorPosX *= mouseSensitivity;
    deltaCursorPosY *= mouseSensitivity;
}
