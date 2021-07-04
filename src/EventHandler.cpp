#include "EventHandler.h"

#include <iostream>

CameraActionMap EventHandler::cameraActionMap = {{CameraAction::Forward, false}, {CameraAction::Backward, false},
                                                 {CameraAction::Up, false},      {CameraAction::Down, false},
                                                 {CameraAction::Left, false},    {CameraAction::Right, false}};

CameraMouseActionMap EventHandler::cameraMouseActionMap = {{CameraMouseAction::DeltaX, 0.0},
                                                           {CameraMouseAction::DeltaY, 0.0}};

WindowActionMap EventHandler::windowActionMap = {{WindowAction::WireFrameToggle, false},
                                                 {WindowAction::MouseCatchToggle, false},
                                                 {WindowAction::WindowShouldClose, false}};

MouseButtonActionMap EventHandler::mouseButtonActionMap = {{MouseButtonAction::LeftClicked, false},
                                                           {MouseButtonAction::RightClicked, false},
                                                           {MouseButtonAction::MiddleClicked, false}};

bool EventHandler::firstMouseInput = 1;
float EventHandler::mouseSensitivity = 0.15f;

EventHandler::EventHandler(GLFWwindow *p_window) : window(p_window)
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void EventHandler::handleEvents()
{
    clearActionRegisters();
    glfwPollEvents();
}

void EventHandler::clearActionRegisters()
{
    for (auto &element : cameraMouseActionMap) {
        element.second = 0.0;
    }

    for (auto &element : windowActionMap) {
        element.second = 0.0;
    }

    for (auto &element : mouseButtonActionMap) {
        element.second = 0.0;
    }
}

void EventHandler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Silence warnings of unused variables.
    (void)window;
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        windowActionMap[WindowAction::WindowShouldClose] = 1;
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        windowActionMap[WindowAction::WireFrameToggle] = 1;
    }
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        windowActionMap[WindowAction::MouseCatchToggle] = 1;
        firstMouseInput = 1;
    }

    // Movement press
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        cameraActionMap[CameraAction::Forward] = 1;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        cameraActionMap[CameraAction::Backward] = 1;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        cameraActionMap[CameraAction::Up] = 1;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        cameraActionMap[CameraAction::Down] = 1;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        cameraActionMap[CameraAction::Left] = 1;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        cameraActionMap[CameraAction::Right] = 1;
    }

    // Movement release
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        cameraActionMap[CameraAction::Forward] = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        cameraActionMap[CameraAction::Backward] = 0;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        cameraActionMap[CameraAction::Up] = 0;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        cameraActionMap[CameraAction::Down] = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        cameraActionMap[CameraAction::Left] = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        cameraActionMap[CameraAction::Right] = 0;
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

    cameraMouseActionMap[CameraMouseAction::DeltaX] += deltaCursorPosX;
    cameraMouseActionMap[CameraMouseAction::DeltaY] += deltaCursorPosY;
}

void EventHandler::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    (void)window;
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        mouseButtonActionMap[MouseButtonAction::LeftClicked] = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouseButtonActionMap[MouseButtonAction::RightClicked] = true;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        mouseButtonActionMap[MouseButtonAction::MiddleClicked] = true;
}

const CameraActionMap &EventHandler::getCameraActionMap() const
{
    return cameraActionMap;
}

WindowActionMap &EventHandler::getWindowActionMap() const
{
    return windowActionMap;
}

const MouseButtonActionMap &EventHandler::getMouseButtonActionMap() const
{
    return mouseButtonActionMap;
}

const CameraMouseActionMap &EventHandler::getCameraMouseActionMap() const
{
    return cameraMouseActionMap;
}

void EventHandler::setFirstMouseInput(bool val)
{
    firstMouseInput = val;
}
