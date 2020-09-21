#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"
#include "EventHandler.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "defines.h"
#include "Light.h"
#include "Framebuffer.h"


class Controller {

public:
    Controller();
    ~Controller();

    void run();

    static void error_callback(int error, const char* description);

private:

    void limit_framerate();

    void updateWindowSize(ShaderProgram *pp_program);

    void renderImGui(std::vector<Entity> *entites, PointLight *pointLight, glm::vec3 *lightColor, bool *rotateLightSource);

    Window *gameWindow;
    EventHandler *gameEventHandler;
    Camera *camera;

    Framebuffer *pp_framebuffer;

    const uint16_t MAX_FPS = 60;
    double deltaTime;

    bool wireFrameMode = 0;

};
