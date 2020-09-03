#pragma once

#include <glm/glm.hpp>

class Camera {

public:

    Camera(float fov, int width, int height);
    ~Camera() = default;

    glm::mat4 getViewProj();
    void updateVPM();
    void updateAspectRatio(int width, int height);

    void translate(glm::vec3 vector);
    

private:

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::mat4 viewProjectionMatrix;

    float fov;

};
