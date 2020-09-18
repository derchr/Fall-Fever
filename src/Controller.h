#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"
#include "EventHandler.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "Entity.h"
#include "defines.h"


class Controller {

public:
    Controller();
    ~Controller();

    void run();

    static void error_callback(int error, const char* description);

private:

    void limit_framerate();

    void updateWindowSize();

    void renderImGui(std::vector<Entity> *entites, glm::vec3 *lightColor, bool *rotateLightSource);

    Window *gameWindow;
    EventHandler *gameEventHandler;
    Camera *camera;

    const uint16_t MAX_FPS = 60;
    double deltaTime;

    bool wireFrameMode = 0;

};
