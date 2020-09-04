#pragma once

#include <GLFW/glfw3.h>

#include "eventActions.h"

class EventHandler {

public:

    EventHandler(GLFWwindow *p_window);
    ~EventHandler() = default;

    void handleEvents();

    bool * getCameraActionRegister() { return actionCameraRegister; }
    float getCursorDeltaX() { return deltaCursorPosX; }
    float getCursorDeltaY() { return deltaCursorPosY; }

    void setFirstMouseInput(bool val) { firstMouseInput = val; }
    void setLastCursorPos(float posX, float posY) { lastCursorPosX=posX; lastCursorPosY=posY; }

private:

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

    void updateMouseDelta();

    bool actionCameraRegister[CAMERA_ACTION_NUM_ITEMS] = {0};

    GLFWwindow *window;

    static bool wireFrameMode;

    float lastCursorPosX = 0.0f;
    float lastCursorPosY = 0.0f;
    float deltaCursorPosX = 0.0f;
    float deltaCursorPosY = 0.0f;

    float mouseSensitivity = 0.08f;

    bool firstMouseInput = 1;

};