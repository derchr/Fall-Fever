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
#include "World.h"


class Controller
{
public:
    Controller();
    ~Controller();

    void run();

    static ShaderProgram* getShaderProgramByName(std::vector<ShaderProgram*> shaderPrograms, const char *name);
    static void error_callback(int error, const char *description);
    
    void setMaxFps(uint16_t fps);

private:
    void limit_framerate();

    void updateWindowSize();
    void updateExposure(ShaderProgram *shaderProgram);

    ShaderProgram* getShaderProgramByName(const char *name);

    void renderImGui(World &world, PointLight *pointLight, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource, ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows);

private:
    Window *gameWindow;
    EventHandler *gameEventHandler;
    Camera *camera;

    std::vector<ShaderProgram*> shaderPrograms;

    Framebuffer *pp_framebuffer;

    Menu *menu;

    uint16_t MAX_FPS = 60;
    double deltaTime;

    float exposure = 1.0f;
};
