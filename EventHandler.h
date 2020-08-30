#pragma once

#include <GLFW/glfw3.h>

class EventHandler {

public:

    void handleEvents(GLFWwindow *window);
    bool gameShouldTerminate;

private:


};