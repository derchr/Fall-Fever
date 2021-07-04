#pragma once

#include "Camera.h"
#include "Entity.h"
#include "EventHandler.h"
#include "FrameBuffer.h"
#include "Light.h"
#include "Menu.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "World.h"

class Controller
{
public:
    Controller();
    ~Controller();

    void run();

    void setMaxFps(uint16_t fps);

    static ShaderProgram *getShaderProgramByName(std::vector<ShaderProgram *> shaderPrograms, const std::string &name);

private:
    void limit_framerate();

    void updateWindowDimensions();
    void updateExposure(ShaderProgram *shaderProgram);

    ShaderProgram *getShaderProgramByName(const std::string &name);

    void renderImGui(World *world, glm::vec3 *lightColor, bool *rotateEntity, bool *rotateLightSource,
                     ShaderProgram *postProcessingProgram, float *intensity, bool *drawShadows);

    Window *m_gameWindow;
    EventHandler *m_gameEventHandler;

    World *m_world;

    Camera *m_camera;
    Menu *m_menu;

    std::vector<ShaderProgram *> m_shaderPrograms;

    FrameBuffer *m_postProcessFrameBuffer;

    uint16_t m_MAX_FPS = 60;
    double m_deltaTime;

    float m_exposure = 1.0f;
};
