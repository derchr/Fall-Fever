#pragma once

#include <GLFW/glfw3.h>

#include "eventActions.h"

class EventHandler
{
public:
    EventHandler(GLFWwindow *p_window);
    ~EventHandler() = default;

    void handleEvents();

    bool *getCameraActionRegister();
    bool *getWindowActionRegister();
    bool *getMouseButtonActionRegister();
    double *getCursorDelta();
    void setFirstMouseInput(bool val);

private:
    void clearActionRegisters();

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

private:
    static bool cameraActionRegister[CAMERA_ACTION_NUM_ITEMS];
    static double cameraMouseActionRegister[CAMERA_MOUSE_ACTION_NUM_ITEMS];
    static bool windowActionRegister[WINDOW_ACTION_NUM_ITEMS];
    static bool mouseButtonActionRegister[MOUSE_BUTTON_ACTION_NUM_ITEMS];

    GLFWwindow *window;

    static float mouseSensitivity;
    static bool firstMouseInput;
};
