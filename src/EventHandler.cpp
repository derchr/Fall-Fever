#include "EventHandler.h"

#include <iostream>

bool EventHandler::cameraActionRegister[CAMERA_ACTION_NUM_ITEMS] = {0};
double EventHandler::cameraMouseActionRegister[CAMERA_MOUSE_ACTION_NUM_ITEMS] = {0.0, 0.0};
bool EventHandler::windowActionRegister[WINDOW_ACTION_NUM_ITEMS] = {0};

bool EventHandler::firstMouseInput = 1;
float EventHandler::mouseSensitivity = 0.5f;


EventHandler::EventHandler(GLFWwindow *p_window) :
    window(p_window)
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
}

void EventHandler::handleEvents()
{
    clearActionRegisters();
    glfwPollEvents();
}

void EventHandler::clearActionRegisters()
{
    //std::fill_n(cameraActionRegister, CAMERA_ACTION_NUM_ITEMS, 0);
    std::fill_n(cameraMouseActionRegister, CAMERA_MOUSE_ACTION_NUM_ITEMS, 0.0);
    std::fill_n(windowActionRegister, WINDOW_ACTION_NUM_ITEMS, 0);
}

void EventHandler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Silence warnings of unused variables.
    (void)window;
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        windowActionRegister[wireFrameToggle] = 1;
    }
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        windowActionRegister[mouseCatchToggle] = 1;
        firstMouseInput = 1;
    }

    // Movement press
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        cameraActionRegister[cameraForward] = 1;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        cameraActionRegister[cameraBackward] = 1;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        cameraActionRegister[cameraUp] = 1;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        cameraActionRegister[cameraDown] = 1;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        cameraActionRegister[cameraLeft] = 1;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        cameraActionRegister[cameraRight] = 1;
    }

    // Movement release
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        cameraActionRegister[cameraForward] = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        cameraActionRegister[cameraBackward] = 0;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        cameraActionRegister[cameraUp] = 0;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        cameraActionRegister[cameraDown] = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        cameraActionRegister[cameraLeft] = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        cameraActionRegister[cameraRight] = 0;
    }
}

void EventHandler::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    (void)window;

    static double lastCursorPosX = 0.0;
    static double lastCursorPosY = 0.0;

    double deltaCursorPosX = xpos - lastCursorPosX;
    double deltaCursorPosY = -(ypos - lastCursorPosY);

    lastCursorPosX = xpos;
    lastCursorPosY = ypos;

    // Check if this is the first VALID mouse event after window being resized
    if (firstMouseInput && !(deltaCursorPosX == 0 && deltaCursorPosY == 0)) {
        firstMouseInput = 0;
        deltaCursorPosX = 0.0;
        deltaCursorPosY = 0.0;
    }

    deltaCursorPosX *= mouseSensitivity;
    deltaCursorPosY *= mouseSensitivity;

    cameraMouseActionRegister[cameraMouseDeltaX] = deltaCursorPosX;
    cameraMouseActionRegister[cameraMouseDeltaY] = deltaCursorPosY;
}
