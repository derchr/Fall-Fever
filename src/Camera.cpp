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

void Camera::updatePositionFromKeyboardInput(bool *actionCameraRegister, float deltaTime) {
    glm::vec3 frontVecWithoutY = glm::vec3(frontVec.x, 0.0f, frontVec.z);

    glm::vec3 deltaPos = glm::vec3(0.0f, 0.0f, 0.0f);

    if(actionCameraRegister[cameraForward])
        deltaPos += speed * deltaTime * glm::normalize(frontVecWithoutY);
    if(actionCameraRegister[cameraBackward])
        deltaPos -= speed * deltaTime * glm::normalize(frontVecWithoutY);
    if(actionCameraRegister[cameraLeft])
        deltaPos -= speed * deltaTime * glm::normalize(glm::cross(frontVec, upVec));
    if(actionCameraRegister[cameraRight])
        deltaPos += speed * deltaTime * glm::normalize(glm::cross(frontVec, upVec));
    if(actionCameraRegister[cameraUp])
        deltaPos += speed * deltaTime * upVec;
    if(actionCameraRegister[cameraDown])
        deltaPos -= speed * deltaTime * upVec;

    position += deltaPos;
}

void Camera::updateDirectionFromMouseInput(float deltaCursorX, float deltaCursorY) {
    if(deltaCursorX==0 && deltaCursorY==0) return;
    yaw += deltaCursorX;
    pitch += deltaCursorY;
    
    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec = glm::normalize(direction);
}
