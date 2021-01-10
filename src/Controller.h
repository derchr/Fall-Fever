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
#include "Menu.h"


class Controller
{
public:
    Controller();
    ~Controller();

    void run();

    static void error_callback(int error, const char *description);

private:
    void limit_framerate();

    void updateWindowSize(ShaderProgram *pp_program);
    void updateExposure(ShaderProgram *shaderProgram);

    void renderImGui(std::vector<Entity> *entites, PointLight *pointLight, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource, ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows);

    Window *gameWindow;
    EventHandler *gameEventHandler;
    Camera *camera;

    /*struct shaderProgram {

    }*/

    ShaderProgram *shaderProgram;
    ShaderProgram *lightProgram;
    ShaderProgram *skyboxProgram;
    ShaderProgram *postProcessingProgram;
    ShaderProgram *menuProgram;
    ShaderProgram *directionalShadowDepthProgram;
    ShaderProgram *pointShadowDepthProgram;

    Framebuffer *pp_framebuffer;

    Menu *menu;

    const uint16_t MAX_FPS = 60;
    double deltaTime;

    bool wireFrameMode = 0;

    float exposure = 1.0f;
};
