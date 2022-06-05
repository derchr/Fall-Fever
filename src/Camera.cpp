#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspectRatio)
{
    this->m_fov = fov;
    m_viewMatrix = glm::mat4(1.0f);
    updateAspectRatio(aspectRatio);
    updateVPM();
}

void Camera::updateVPM()
{
    m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}

void Camera::updateAspectRatio(float aspectRatio)
{
    // m_projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.f, 100.0f);
    m_projectionMatrix = glm::perspective(m_fov / 2.0f, aspectRatio, 0.1f, 1000.0f);
    updateVPM();
}

void Camera::translate(glm::vec3 translateVector)
{
    m_position += translateVector;
    m_viewMatrix = glm::translate(m_viewMatrix, translateVector * -1.0f);
}

void Camera::lookAtTarget(glm::vec3 target)
{
    m_viewMatrix = glm::lookAt(m_position, target, m_upVec);
}

void Camera::lookForward()
{
    m_viewMatrix = glm::lookAt(m_position, m_position + m_frontVec, m_upVec);
}

// void Camera::updatePositionFromKeyboardInput(const CameraActionMap &cameraActionMap, float deltaTime)
// {
//     glm::vec3 frontVecWithoutY = glm::vec3(m_frontVec.x, 0.0f, m_frontVec.z);

//     glm::vec3 deltaPos = glm::vec3(0.0f, 0.0f, 0.0f);

//     if (cameraActionMap.at(CameraAction::Forward)) {
//         deltaPos += m_speed * deltaTime * glm::normalize(frontVecWithoutY);
//     }
//     if (cameraActionMap.at(CameraAction::Backward)) {
//         deltaPos -= m_speed * deltaTime * glm::normalize(frontVecWithoutY);
//     }
//     if (cameraActionMap.at(CameraAction::Left)) {
//         deltaPos -= m_speed * deltaTime * glm::normalize(glm::cross(m_frontVec, m_upVec));
//     }
//     if (cameraActionMap.at(CameraAction::Right)) {
//         deltaPos += m_speed * deltaTime * glm::normalize(glm::cross(m_frontVec, m_upVec));
//     }
//     if (cameraActionMap.at(CameraAction::Up)) {
//         deltaPos += m_speed * deltaTime * m_upVec;
//     }
//     if (cameraActionMap.at(CameraAction::Down)) {
//         deltaPos -= m_speed * deltaTime * m_upVec;
//     }

//     m_position += deltaPos;
// }

// void Camera::updateDirectionFromMouseInput(const CameraMouseActionMap &cameraMouseActionMap)
// {

//     if (cameraMouseActionMap.at(CameraMouseAction::DeltaX) == 0 &&
//         cameraMouseActionMap.at(CameraMouseAction::DeltaY) == 0) {
//         return;
//     }

//     m_yaw += cameraMouseActionMap.at(CameraMouseAction::DeltaX);
//     m_pitch += cameraMouseActionMap.at(CameraMouseAction::DeltaY);

//     if (m_pitch > 89.0f) {
//         m_pitch = 89.0f;
//     }
//     if (m_pitch < -89.0f) {
//         m_pitch = -89.0f;
//     }

//     glm::vec3 direction;
//     direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
//     direction.y = sin(glm::radians(m_pitch));
//     direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
//     m_frontVec = glm::normalize(direction);
// }

void Camera::setPosition(glm::vec3 position)
{
    this->m_position = position;
}

glm::mat4 Camera::getView()
{
    return m_viewMatrix;
}

glm::mat4 Camera::getProj()
{
    return m_projectionMatrix;
}

glm::mat4 Camera::getViewProj()
{
    return m_viewProjectionMatrix;
}

glm::vec3 Camera::getPosition()
{
    return m_position;
}

glm::vec3 Camera::getDirection()
{
    return m_frontVec;
}
