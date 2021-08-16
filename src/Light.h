#pragma once

#include "Entity.h"

#include <glm/glm.hpp>
#include <string>

#define NUM_POINT_LIGHTS 1

class ShaderProgram;

class Light : public Entity
{
public:
    struct Prototype
    {
        Prototype(const std::string &name, const std::string &parent, glm::vec3 color, float intensity)
            : name(name), parent(parent), color(color), intensity(intensity)
        {}
        virtual ~Prototype() = default;

        std::string name;
        std::string parent;
        glm::vec3 color;
        float intensity;
    };

    Light(const std::string &name, glm::vec3 color, float intensity, ShaderProgram *shaderProgram);
    virtual ~Light() = 0;

    virtual void update() = 0;

    void setActive(bool active);
    void setColor(glm::vec3 color);
    void setIntensity(float intensity);
    void setShaderProgram(ShaderProgram *shaderProgram);

    glm::vec3 getColor();

protected:
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
    struct Prototype : public Light::Prototype
    {
        Prototype(const std::string &name, const std::string &parent, glm::vec3 position, glm::vec3 color,
                  float intensity)
            : Light::Prototype(name, parent, color, intensity), position(position)
        {}
        glm::vec3 position;
    };

    PointLight(Prototype prototype, ShaderProgram *shaderProgram);
    ~PointLight() override = default;

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
    struct Prototype : public Light::Prototype
    {
        Prototype(const std::string &name, glm::vec3 direction, glm::vec3 color, float intensity)
            : Light::Prototype(name, "", color, intensity), direction(direction)
        {}
        glm::vec3 direction;
    };

    DirectionalLight(Prototype prototype, ShaderProgram *shaderProgram);
    ~DirectionalLight() override = default;

    void setDirection(glm::vec3 direction);

    glm::vec3 getDirection();

private:
    void update() override;

    glm::vec3 m_direction;
};
