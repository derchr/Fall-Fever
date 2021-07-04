#pragma once

#include <glm/glm.hpp>
#include <string>

#define NUM_POINT_LIGHTS 1

class ShaderProgram;

class Light
{
public:
    virtual ~Light()
    {}

    virtual void update() = 0;

    void setActive(bool active);
    void setColor(glm::vec3 color);
    void setIntensity(float intensity);
    void setShaderProgram(ShaderProgram *shaderProgram);

    glm::vec3 getColor();

protected:
    Light(glm::vec3 color, float intensity, ShaderProgram *shaderProgram);

    ShaderProgram *m_shaderProgram;

    uint32_t m_id;
    static uint32_t s_idCounter;

    bool m_isActive;
    bool m_shouldCastShadow = true;

    float m_intensity;

    // Color
    glm::vec3 m_lightColor;
};

class PointLight : public Light
{
public:
    PointLight(glm::vec3 position, glm::vec3 color, float intensity, ShaderProgram *shaderProgram);
    ~PointLight() = default;

    void setPosition(glm::vec3 position);

    glm::vec3 getPosition();

private:
    void update() override;
    std::string getStructMemberName();

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, ShaderProgram *shaderProgram);

    void setDirection(glm::vec3 direction);

    glm::vec3 getDirection();

private:
    void update() override;

    glm::vec3 m_direction;
};
