#pragma once

#include <glm/glm.hpp>

class Camera {

public:

    Camera(float fov, int width, int height);
    ~Camera() = default;

    glm::mat4 getViewProj();
    void updateVPM();
    void updateAspectRatio(int width, int height);
    void updatePositionFromKeyboardInput(bool *actionCameraRegister, float deltaTime);
    void updateDirectionFromMouseInput(float deltaCursorX, float deltaCursorY);

    void translate(glm::vec3 translateVector);

    void setPosition(glm::vec3 position) { this->position = position; }
    void lookAtTarget(glm::vec3 target);
    void lookForward();
    

private:

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::mat4 viewProjectionMatrix;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upVec = glm::vec3(0.0f, 1.0f,  0.0f);

    float pitch;
    float yaw = -90.0f;

    float speed = 2.0f;

    float fov;

};
