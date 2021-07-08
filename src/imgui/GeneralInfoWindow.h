#pragma once

#include "Window.h"

#include <glm/glm.hpp>

class World;
class ShaderProgram;
class Controller;

namespace Imgui {

class GeneralInfoWindow : public Window
{
public:
    GeneralInfoWindow(Controller *controller, World *world, ShaderProgram *postProcessingProgram, bool *rotateEntity,
                      bool *drawShadows, bool *rotateLightSource, glm::vec3 *lightColor, float *exposure,
                      float *intensity);

private:
    void addWidgets() override;

    Controller *m_controller;
    World *m_world;
    ShaderProgram *m_postProcessingProgram;

    bool *m_rotateEntity;
    bool *m_drawShadows;
    bool *m_rotateLightSource;

    float m_rotation = 0.0f;
    float m_translation[3] = {0.0f, 1.0f, 0.0f};
    float m_scale = 0.6f;
    float m_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec3 *m_lightColor;
    float *m_exposure;
    float *m_intensity;
};

}; // namespace Imgui
