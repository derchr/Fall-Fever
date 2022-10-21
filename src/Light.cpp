#include "Light.h"
#include "ShaderProgram.h"

#include <string>

uint32_t Light::s_idCounter = 0;

Light::Light(const std::string &name, glm::vec3 color, float intensity, ShaderProgram *shaderProgram)
    : m_shaderProgram(shaderProgram), m_intensity(intensity), m_lightColor(color * intensity)
{
    m_id = s_idCounter++;
}

Light::~Light() = default;

glm::vec3 Light::getColor()
{
    return m_lightColor;
}

void Light::setShaderProgram(ShaderProgram *shaderProgram)
{
    this->m_shaderProgram = shaderProgram;
    update();
}

void Light::setColor(glm::vec3 color)
{
    m_lightColor = color * m_intensity;
    update();
}

void Light::setIntensity(float intensity)
{
    this->m_intensity = intensity;
}

void Light::setActive(bool active)
{
    m_isActive = active;
    update();
}

PointLight::PointLight(Prototype prototype, ShaderProgram *shaderProgram)
    : Light(prototype.name, prototype.color, prototype.intensity, shaderProgram), m_position(prototype.position)
{
    update();
}

void PointLight::update()
{
    m_shaderProgram->bind();

    m_shaderProgram->setUniform((getStructMemberName() + "isActive").c_str(), m_isActive);
    m_shaderProgram->setUniform((getStructMemberName() + "position").c_str(), m_position);
    m_shaderProgram->setUniform((getStructMemberName() + "color").c_str(), m_lightColor);

    m_shaderProgram->unbind();
}

std::string PointLight::getStructMemberName()
{
    // id - 1 because id 0 is always the DirectionalLight!
    std::string temp = "u_pointLight[" + std::to_string(m_id - 1) + "].";
    return temp;
}

glm::vec3 PointLight::getPosition()
{
    return m_position;
}

void PointLight::setPosition(glm::vec3 position)
{
    this->m_position = position;
    update();
}

DirectionalLight::DirectionalLight(Prototype prototype, ShaderProgram *shaderProgram)
    : Light(prototype.name, prototype.color, prototype.intensity, shaderProgram), m_direction(prototype.direction)
{
    update();
}

void DirectionalLight::update()
{
    m_shaderProgram->bind();

    m_shaderProgram->setUniform("u_directionalLight.isActive", m_isActive);
    m_shaderProgram->setUniform("u_directionalLight.direction", m_direction);
    m_shaderProgram->setUniform("u_directionalLight.color", m_lightColor);

    m_shaderProgram->unbind();
}

void DirectionalLight::setDirection(glm::vec3 direction)
{
    this->m_direction = direction;
    update();
}

glm::vec3 DirectionalLight::getDirection()
{
    return m_direction;
}
