#pragma once

#include <GLFW/glfw3.h>

#include "eventActions.h"

class EventHandler {

public:

    EventHandler(GLFWwindow *p_window);
    ~EventHandler() = default;

    void handleEvents();

    bool * getCameraActionRegister() { return actionCameraRegister; }

private:

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    bool actionCameraRegister[CAMERA_ACTION_NUM_ITEMS] = {0};

    GLFWwindow *window;

    static bool wireFrameMode;

};