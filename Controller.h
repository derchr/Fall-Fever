#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"
#include "EventHandler.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "defines.h"

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

    bool wireFrameMode = 0;

};
