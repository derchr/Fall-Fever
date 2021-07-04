#include "Camera.h"
#include "definitions/eventActions.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspectRatio)
{
    this->fov = fov;
    viewMatrix = glm::mat4(1.0f);
    updateAspectRatio(aspectRatio);
    updateVPM();
}

void Camera::updateVPM()
{
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::updateAspectRatio(float aspectRatio)
{
    // projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.f, 100.0f);
    projectionMatrix = glm::perspective(fov / 2.0f, aspectRatio, 0.1f, 1000.0f);
    updateVPM();
}

void Camera::translate(glm::vec3 translateVector)
{
    position += translateVector;
    viewMatrix = glm::translate(viewMatrix, translateVector * -1.0f);
}

void Camera::lookAtTarget(glm::vec3 target)
{
    viewMatrix = glm::lookAt(position, target, upVec);
}

void Camera::lookForward()
{
    viewMatrix = glm::lookAt(position, position + frontVec, upVec);
}

void Camera::updatePositionFromKeyboardInput(const CameraActionMap &cameraActionMap, float deltaTime)
{
    glm::vec3 frontVecWithoutY = glm::vec3(frontVec.x, 0.0f, frontVec.z);

    glm::vec3 deltaPos = glm::vec3(0.0f, 0.0f, 0.0f);

    if (cameraActionMap.at(CameraAction::Forward)) {
        deltaPos += speed * deltaTime * glm::normalize(frontVecWithoutY);
    }
    if (cameraActionMap.at(CameraAction::Backward)) {
        deltaPos -= speed * deltaTime * glm::normalize(frontVecWithoutY);
    }
    if (cameraActionMap.at(CameraAction::Left)) {
        deltaPos -= speed * deltaTime * glm::normalize(glm::cross(frontVec, upVec));
    }
    if (cameraActionMap.at(CameraAction::Right)) {
        deltaPos += speed * deltaTime * glm::normalize(glm::cross(frontVec, upVec));
    }
    if (cameraActionMap.at(CameraAction::Up)) {
        deltaPos += speed * deltaTime * upVec;
    }
    if (cameraActionMap.at(CameraAction::Down)) {
        deltaPos -= speed * deltaTime * upVec;
    }

    position += deltaPos;
}

void Camera::updateDirectionFromMouseInput(const CameraMouseActionMap &cameraMouseActionMap)
{

    if (cameraMouseActionMap.at(CameraMouseAction::DeltaX) == 0 &&
        cameraMouseActionMap.at(CameraMouseAction::DeltaY) == 0) {
        return;
    }

    yaw += cameraMouseActionMap.at(CameraMouseAction::DeltaX);
    pitch += cameraMouseActionMap.at(CameraMouseAction::DeltaY);

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec = glm::normalize(direction);
}

void Camera::setPosition(glm::vec3 position)
{
    this->position = position;
}

glm::mat4 Camera::getView()
{
    return viewMatrix;
}

glm::mat4 Camera::getProj()
{
    return projectionMatrix;
}

glm::mat4 Camera::getViewProj()
{
    return viewProjectionMatrix;
}

glm::vec3 Camera::getPosition()
{
    return position;
}

glm::vec3 Camera::getDirection()
{
    return frontVec;
}
