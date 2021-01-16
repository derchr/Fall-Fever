#include "Light.h"

#include <string>

uint32_t Light::id_counter = 0;

// Light

Light::Light(glm::vec3 color, float intensity, ShaderProgram* shaderProgram) :
     shaderProgram(shaderProgram),
     intensity(intensity)
{
    id = id_counter++;
    lightColor = color * intensity;
    diffuseColor = lightColor * glm::vec3(1.0f);
    ambientColor = diffuseColor * glm::vec3(0.002f);
    specularColor = lightColor * glm::vec3(1.0f);
}


glm::vec3 Light::getColor()
{
    return lightColor;
}

void Light::setShaderProgram(ShaderProgram* shaderProgram)
{
    this->shaderProgram = shaderProgram;
    update();
}

void Light::setColor(glm::vec3 color)
{
    lightColor = color * intensity;
    diffuseColor = lightColor * glm::vec3(1.0f);
    ambientColor = diffuseColor * glm::vec3(0.002f);
    specularColor = lightColor * glm::vec3(1.0f);
    update();
}

void Light::setIntensity(float intensity)
{
    this->intensity = intensity;
}


void Light::setActive(bool active)
{
    isActive = active;
    update();
}

// PointLight

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity, ShaderProgram *shaderProgram) :
    Light(color, intensity, shaderProgram),
    position(position)
{
    // Empty
}

void PointLight::update()
{
    shaderProgram->bind();

    shaderProgram->setUniform((getStructMemberName() + "isActive").c_str(), isActive);
    shaderProgram->setUniform((getStructMemberName() + "position").c_str(), position);
    shaderProgram->setUniform((getStructMemberName() + "ambient").c_str(), ambientColor);
    shaderProgram->setUniform((getStructMemberName() + "diffuse").c_str(), diffuseColor);
    shaderProgram->setUniform((getStructMemberName() + "specular").c_str(), specularColor);
    shaderProgram->setUniform((getStructMemberName() + "K_q").c_str(), K_q);

    shaderProgram->unbind();
}

std::string PointLight::getStructMemberName()
{
    std::string temp = "u_pointLight[" + std::to_string(id) + "].";
    return temp;
}

glm::vec3 PointLight::getPosition()
{
    return position;
}

void PointLight::setPosition(glm::vec3 position)
{
    this->position = position;
    update();
}

void PointLight::setParameters(float K_q)
{
    this->K_q = K_q;
}

// DirectionalLight

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, ShaderProgram *shaderProgram) :
    Light(color, intensity, shaderProgram),
    direction(direction)
{
    // Empty
}

void DirectionalLight::update()
{
    shaderProgram->bind();

    shaderProgram->setUniform("u_directionalLight.isActive", isActive);
    shaderProgram->setUniform("u_directionalLight.direction", direction);
    shaderProgram->setUniform("u_directionalLight.ambient", ambientColor);
    shaderProgram->setUniform("u_directionalLight.diffuse", diffuseColor);
    shaderProgram->setUniform("u_directionalLight.specular", specularColor);

    shaderProgram->unbind();
}

void DirectionalLight::setDirection(glm::vec3 direction)
{
    this->direction = direction;
    update();
}

glm::vec3 DirectionalLight::getDirection()
{
    return direction;
}
