#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"

#define NUM_POINT_LIGHTS 16

class Light {

public:

    virtual void update() = 0;

    void setActive(bool active) { isActive = active; }
    void setColor(glm::vec3 color) {
        lightColor = color; 
        update();
    }
    void setShaderProgram(ShaderProgram *shaderProgram) {
        this->shaderProgram = shaderProgram;
        update();
    }

protected:

    Light() = default;
    Light(ShaderProgram *shaderProgram) : shaderProgram(shaderProgram) {}

    ShaderProgram *shaderProgram;

    bool isActive = false;

    // Color
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);
    glm::vec3 specularColor = glm::vec3(1.0f);

};

class PointLight : public Light {

public:

    PointLight() = default;
    PointLight(ShaderProgram *shaderProgram);

    void setPosition(glm::vec3 position) { 
        this->position = position;
        update();
    }
    void setId(unsigned int id) { lightId = id; }

    void update();

private:

    unsigned int lightId;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float K_c = 1.0f;
    float K_l = 0.09f;
    float K_q = 0.032f;

};
