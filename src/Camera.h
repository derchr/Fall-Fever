#pragma once

#include "definitions/eventActions.h"

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(float fov, float aspectRatio);
    ~Camera() = default;

    void updateVPM();
    void updateAspectRatio(float aspectRatio);
    void updatePositionFromKeyboardInput(const CameraActionMap &cameraActionMap, float deltaTime);
    void updateDirectionFromMouseInput(const CameraMouseActionMap &cameraMouseActionMap);

    void translate(glm::vec3 translateVector);

    void lookAtTarget(glm::vec3 target);
    void lookForward();

    glm::mat4 getView();
    glm::mat4 getProj();
    glm::mat4 getViewProj();
    glm::vec3 getPosition();
    glm::vec3 getDirection();

    void setPosition(glm::vec3 position);

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewProjectionMatrix;

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_upVec = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_pitch = 0.0f;
    float m_yaw = -90.0f;

    float m_speed = 2.0f;

    float m_fov;
};
