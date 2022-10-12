#pragma once

#include "Window.h"

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(float fov, float aspectRatio);

    void updateVPM();
    void updateAspectRatio(float aspectRatio);
    void updatePositionFromKeyboardInput(KeyInput const &key_input, float deltaTime);
    void updateDirectionFromMouseInput(MouseCursorInput const &mouse_cursor_input);

    void translate(glm::vec3 translateVector);

    void lookAtTarget(glm::vec3 target);
    void lookForward();

    [[nodiscard]] auto getView() const -> glm::mat4;
    [[nodiscard]] auto getProj() const -> glm::mat4;
    [[nodiscard]] auto getViewProj() const -> glm::mat4;
    [[nodiscard]] auto getPosition() const -> glm::vec3;
    [[nodiscard]] auto getDirection() const -> glm::vec3;

    void setPosition(glm::vec3 position);

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewProjectionMatrix;

    glm::vec3 m_position = glm::vec3(0., 0., 0.);
    glm::vec3 m_front_vec = glm::vec3(0., 0., -1.);

    static constexpr glm::vec3 UP_VEC = glm::vec3(0., 1., 0.);
    static constexpr float SPEED = .5;
    static constexpr float DEFAULT_YAW = -90.;

    float m_pitch{};
    float m_yaw = DEFAULT_YAW;

    float m_fov;

    bool m_accellerate = false;
};
