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

    Framebuffer *pp_framebuffer;

    const uint16_t MAX_FPS = 60;
    double deltaTime;

    bool wireFrameMode = 0;

    float exposure = 1.0f;
    
    float loadingScreenVerticesData[12] = {
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };
    unsigned int loadingScreenIndicesData[6] = {
        0, 1, 2,
        0, 3, 1
    };
    float loadingScreenTextureCoordinates[8] = {
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f
    };
};
