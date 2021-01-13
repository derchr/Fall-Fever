#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"

class Camera
{
public:
    Camera(float fov, float aspectRatio);
    ~Camera() = default;

    void updateVPM();
    void updateAspectRatio(float aspectRatio);
    void updatePositionFromKeyboardInput(bool *actionCameraRegister, float deltaTime);
    void updateDirectionFromMouseInput(double *cameraMouseActionRegister);

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
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upVec = glm::vec3(0.0f, 1.0f,  0.0f);

    float pitch = 0.0f;
    float yaw = -90.0f;

    float speed = 2.0f;

    float fov;
};


