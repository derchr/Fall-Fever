#include "Camera.h"
#include "eventActions.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(float fov, int width, int height) {
    this->fov = fov;
    viewMatrix = glm::mat4(1.0f);
    updateAspectRatio(width, height);
    updateVPM();
}

glm::mat4 Camera::getViewProj() {
    return viewProjectionMatrix;
}

void Camera::updateVPM() {
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::updateAspectRatio(int width, int height) {
    //projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.f, 100.0f);
    projectionMatrix = glm::perspective(fov/2.0f, (float)width / (float)height, 0.1f, 1000.0f);
    updateVPM();
}

void Camera::translate(glm::vec3 translateVector) {
    position += translateVector;
    viewMatrix = glm::translate(viewMatrix, translateVector * -1.0f);
}

void Camera::lookAtTarget(glm::vec3 target) {
    viewMatrix = glm::lookAt(position, target, upVec);
}

void Camera::lookForward() {
    viewMatrix = glm::lookAt(position, position + frontVec, upVec);
}

void Camera::updatePositionFromKeyboardInput(bool *actionCameraRegister) {
    if(actionCameraRegister[cameraForward])
        position += speed * frontVec;
    if(actionCameraRegister[cameraBackward])
        position -= speed * frontVec;
    if(actionCameraRegister[cameraLeft])
        position -= speed * glm::cross(frontVec, upVec);
    if(actionCameraRegister[cameraRight])
        position += speed * glm::cross(frontVec, upVec);
    if(actionCameraRegister[cameraUp])
        position += speed * upVec;
    if(actionCameraRegister[cameraDown])
        position -= speed * upVec;
}
