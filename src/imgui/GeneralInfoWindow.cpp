#include "GeneralInfoWindow.h"
#include "../Controller.h"
#include "../Entity.h"
#include "../ShaderProgram.h"
#include "../World.h"

#include <imgui.h>
#include <math.h>

Imgui::GeneralInfoWindow::GeneralInfoWindow(Controller *controller, World *world, ShaderProgram *postProcessingProgram,
                                            bool *rotateEntity, bool *drawShadows, bool *rotateLightSource,
                                            glm::vec3 *lightColor, float *exposure, float *intensity)
    : m_controller(controller), m_world(world), m_postProcessingProgram(postProcessingProgram),
      m_rotateEntity(rotateEntity), m_drawShadows(drawShadows), m_rotateLightSource(rotateLightSource),
      m_lightColor(lightColor), m_exposure(exposure), m_intensity(intensity)
{}

void Imgui::GeneralInfoWindow::addWidgets()
{
    ImGui::Begin("Debug Utils");
    ImGui::Text("Object");

    ImGui::SliderFloat("Rotation", &m_rotation, 0, 2 * M_PI);
    ImGui::SliderFloat3("Position", m_translation, -4.0, 4.0);
    ImGui::SliderFloat("Scale", &m_scale, 0.02, 2.0);

    ImGui::Checkbox("Rotate Object", m_rotateEntity);

    Entity *mainObject = m_world->getEntityById(0);
    mainObject->setPosition(glm::vec3(m_translation[0], m_translation[1], m_translation[2]));
    if (!*m_rotateEntity) {
        mainObject->setRotation(glm::vec3(0.f, 1.0f, 0.f), m_rotation);
    }
    mainObject->setScale(m_scale);

    // color picker
    ImGui::Text("\nLight Source");

    m_controller->updateExposure(m_postProcessingProgram);

    ImGui::SliderFloat("Intensity", m_intensity, 0, 250.f);

    ImGui::ColorEdit3("Color", m_color);
    m_lightColor->x = m_color[0];
    m_lightColor->y = m_color[1];
    m_lightColor->z = m_color[2];

    ImGui::Text("\nMiscellaneous");
    ImGui::SliderFloat("Exposure", m_exposure, 0, 5.0f);

    ImGui::Checkbox("Draw Shadows", m_drawShadows);
    ImGui::Checkbox("Rotate Lightsource", m_rotateLightSource);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    ImGui::End();
}