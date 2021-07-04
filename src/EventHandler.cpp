#include "EventHandler.h"

#include "GLFW/glfw3.h"
#include <iostream>

CameraActionMap EventHandler::s_cameraActionMap = {{CameraAction::Forward, false}, {CameraAction::Backward, false},
                                                   {CameraAction::Up, false},      {CameraAction::Down, false},
                                                   {CameraAction::Left, false},    {CameraAction::Right, false}};

CameraMouseActionMap EventHandler::s_cameraMouseActionMap = {{CameraMouseAction::DeltaX, 0.0},
                                                             {CameraMouseAction::DeltaY, 0.0}};

WindowActionMap EventHandler::s_windowActionMap = {{WindowAction::WireFrameToggle, false},
                                                   {WindowAction::MouseCatchToggle, false},
                                                   {WindowAction::WindowShouldClose, false}};

MouseButtonActionMap EventHandler::s_mouseButtonActionMap = {{MouseButtonAction::LeftClicked, false},
                                                             {MouseButtonAction::RightClicked, false},
                                                             {MouseButtonAction::MiddleClicked, false}};

bool EventHandler::s_firstMouseInput = 1;
float EventHandler::s_mouseSensitivity = 0.15f;

EventHandler::EventHandler(GLFWwindow *p_window) : m_window(p_window)
{
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
}

void EventHandler::handleEvents()
{
    clearActionRegisters();
    glfwPollEvents();
}

void EventHandler::clearActionRegisters()
{
    for (auto &element : s_cameraMouseActionMap) {
        element.second = 0.0;
    }

    for (auto &element : s_windowActionMap) {
        element.second = 0.0;
    }

    for (auto &element : s_mouseButtonActionMap) {
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
        s_windowActionMap[WindowAction::WindowShouldClose] = 1;
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        s_windowActionMap[WindowAction::WireFrameToggle] = 1;
    }
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        s_windowActionMap[WindowAction::MouseCatchToggle] = 1;
        s_firstMouseInput = 1;
    }

    // Movement press
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        s_cameraActionMap[CameraAction::Forward] = 1;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        s_cameraActionMap[CameraAction::Backward] = 1;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        s_cameraActionMap[CameraAction::Up] = 1;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        s_cameraActionMap[CameraAction::Down] = 1;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        s_cameraActionMap[CameraAction::Left] = 1;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        s_cameraActionMap[CameraAction::Right] = 1;
    }

    // Movement release
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        s_cameraActionMap[CameraAction::Forward] = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        s_cameraActionMap[CameraAction::Backward] = 0;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        s_cameraActionMap[CameraAction::Up] = 0;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        s_cameraActionMap[CameraAction::Down] = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        s_cameraActionMap[CameraAction::Left] = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        s_cameraActionMap[CameraAction::Right] = 0;
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
    if (s_firstMouseInput && !(deltaCursorPosX == 0 && deltaCursorPosY == 0)) {
        s_firstMouseInput = 0;
        deltaCursorPosX = 0.0;
        deltaCursorPosY = 0.0;
    }

    deltaCursorPosX *= s_mouseSensitivity;
    deltaCursorPosY *= s_mouseSensitivity;

    s_cameraMouseActionMap[CameraMouseAction::DeltaX] += deltaCursorPosX;
    s_cameraMouseActionMap[CameraMouseAction::DeltaY] += deltaCursorPosY;
}

void EventHandler::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    (void)window;
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        s_mouseButtonActionMap[MouseButtonAction::LeftClicked] = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        s_mouseButtonActionMap[MouseButtonAction::RightClicked] = true;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        s_mouseButtonActionMap[MouseButtonAction::MiddleClicked] = true;
}

const CameraActionMap &EventHandler::getCameraActionMap() const
{
    return s_cameraActionMap;
}

WindowActionMap &EventHandler::getWindowActionMap() const
{
    return s_windowActionMap;
}

const MouseButtonActionMap &EventHandler::getMouseButtonActionMap() const
{
    return s_mouseButtonActionMap;
}

const CameraMouseActionMap &EventHandler::getCameraMouseActionMap() const
{
    return s_cameraMouseActionMap;
}

void EventHandler::setFirstMouseInput(bool val)
{
    s_firstMouseInput = val;
}
