#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"
#include "EventHandler.h"

class Controller {

public:
    Controller();
    ~Controller();

    void run();

    static void error_callback(int error, const char* description);

private:

    void limit_framerate();

    Window *gameWindow;
    EventHandler *gameEventHandler;

    const uint16_t MAX_FPS = 60;

    double deltaTime;

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

};
