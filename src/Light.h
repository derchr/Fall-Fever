#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"

#define NUM_POINT_LIGHTS 16

class Light {

public:

    virtual void update() = 0;

    void setActive(bool active) { 
        isActive = active;
        update();
    }
    void setColor(glm::vec3 color) {
        lightColor = color;
        diffuseColor = lightColor * glm::vec3(1.0f);
        ambientColor = diffuseColor * glm::vec3(0.002f);
        specularColor = lightColor * glm::vec3(1.0f);
        update();
    }

    void setShaderProgram(ShaderProgram *shaderProgram) {
        this->shaderProgram = shaderProgram;
        update();
    }

    glm::vec3 getColor() { return lightColor; }

protected:

    Light() = default;
    Light(ShaderProgram *shaderProgram) : shaderProgram(shaderProgram) {}

    ShaderProgram *shaderProgram;

    bool isActive = false;
    bool shouldCastShadow = true;

    // Color
    glm::vec3 lightColor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
    glm::vec3 specularColor;

};

class PointLight : public Light {

public:

    PointLight() = default;
    PointLight(ShaderProgram *shaderProgram);

    void setPosition(glm::vec3 position) { 
        this->position = position;
        update();
    }

    void setParameters(float K_q) { this->K_q = K_q; }

    void setId(unsigned int id) { lightId = id; }


private:

    void update() override;
    std::string _getStructMemberName();
    unsigned int lightId;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    float K_q = 0.032f;

};

class DirectionalLight : public Light {

public:

    DirectionalLight() = default;
    DirectionalLight(ShaderProgram *shaderProgram);

    void setDirection(glm::vec3 direction) { 
        this->direction = direction;
        update();
    }

private:

    void update() override;

    glm::vec3 direction;

};
