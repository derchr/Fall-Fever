#pragma once

#include <GLFW/glfw3.h>

class EventHandler {

public:

    EventHandler(GLFWwindow *p_window);
    ~EventHandler() = default;

    void handleEvents();

private:

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow *window;

    static bool wireFrameMode;

};