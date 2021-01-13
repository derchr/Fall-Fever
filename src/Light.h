#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"

#define NUM_POINT_LIGHTS 16

class Light
{
public:
    virtual ~Light() {}

    virtual void update() = 0;

    void setActive(bool active);
    void setColor(glm::vec3 color);
    void setShaderProgram(ShaderProgram *shaderProgram);

    glm::vec3 getColor();

protected:
    Light(ShaderProgram *shaderProgram) : shaderProgram(shaderProgram) {}

protected:
    ShaderProgram *shaderProgram;

    bool isActive = false;
    bool shouldCastShadow = true;

    // Color
    glm::vec3 lightColor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
    glm::vec3 specularColor;
};

class PointLight : public Light
{
public:
    PointLight(ShaderProgram *shaderProgram);
    ~PointLight() = default;

    void setPosition(glm::vec3 position);
    void setParameters(float K_q);
    void setId(unsigned int id);

    glm::vec3 getPosition();

private:
    void update() override;
    std::string getStructMemberName();

private:
    unsigned int lightId;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    float K_q = 0.032f;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(ShaderProgram *shaderProgram);

    void setDirection(glm::vec3 direction);

private:
    void update() override;

private:
    glm::vec3 direction;
};
