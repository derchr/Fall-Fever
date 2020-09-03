#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(float fov, int width, int height) {
    //projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.f, 100.0f);
    this->fov = fov;
    projectionMatrix = glm::perspective(this->fov/2.0f, (float)width / (float)height, 0.1f, 1000.0f);
    viewMatrix = glm::mat4(1.0f);
    updateVPM();
}

glm::mat4 Camera::getViewProj() {
    return viewProjectionMatrix;
}

void Camera::updateVPM() {
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::updateAspectRatio(int width, int height) {
    projectionMatrix = glm::perspective(fov/2.0f, (float)width / (float)height, 0.1f, 1000.0f);
    updateVPM();
}

void Camera::translate(glm::vec3 vector) {
    viewMatrix = glm::translate(viewMatrix, vector * -1.0f);
}
