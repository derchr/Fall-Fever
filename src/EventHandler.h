#pragma once

#include <GLFW/glfw3.h>

#include "eventActions.h"

class EventHandler {

public:

    EventHandler(GLFWwindow *p_window);
    ~EventHandler() = default;

    void handleEvents();

    bool * getCameraActionRegister() { return cameraActionRegister; }
    bool * getWindowActionRegister() { return windowActionRegister; }

    double * getCursorDelta() { return cameraMouseActionRegister; }

    void setFirstMouseInput(bool val) { firstMouseInput = val; }

private:

    void clearActionRegisters();
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

    static bool cameraActionRegister[CAMERA_ACTION_NUM_ITEMS];
    static double cameraMouseActionRegister[CAMERA_MOUSE_ACTION_NUM_ITEMS];
    static bool windowActionRegister[WINDOW_ACTION_NUM_ITEMS];

    GLFWwindow *window;

    static float mouseSensitivity;

    static bool firstMouseInput;

};