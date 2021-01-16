#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"

#define NUM_POINT_LIGHTS 4

class Light
{
public:
    virtual ~Light() {}

    virtual void update() = 0;

    void setActive(bool active);
    void setColor(glm::vec3 color);
    void setIntensity(float intensity);
    void setShaderProgram(ShaderProgram *shaderProgram);

    glm::vec3 getColor();

protected:
    Light(glm::vec3 color, float intensity, ShaderProgram *shaderProgram);

protected:
    ShaderProgram *shaderProgram;

    uint32_t id;
    static uint32_t id_counter;

    bool isActive;
    bool shouldCastShadow = true;

    float intensity;

    // Color
    glm::vec3 lightColor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
    glm::vec3 specularColor;
};

class PointLight : public Light
{
public:
    PointLight(glm::vec3 position, glm::vec3 color, float intensity, ShaderProgram *shaderProgram);
    ~PointLight() = default;

    void setPosition(glm::vec3 position);
    void setParameters(float K_q);

    glm::vec3 getPosition();

private:
    void update() override;
    std::string getStructMemberName();

private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    float K_q = 0.032f;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, ShaderProgram *shaderProgram);

    void setDirection(glm::vec3 direction);

    glm::vec3 getDirection();

private:
    void update() override;

private:
    glm::vec3 direction;
};
