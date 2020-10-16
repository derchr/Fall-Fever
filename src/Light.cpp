#include "Light.h"

#include <string>

// PointLight

PointLight::PointLight(ShaderProgram *shaderProgram) :
    Light(shaderProgram)
{
    // Empty
}

void PointLight::update()
{
    shaderProgram->bind();

    shaderProgram->setUniform((_getStructMemberName() + "isActive").c_str(), isActive);
    shaderProgram->setUniform((_getStructMemberName() + "position").c_str(), position);
    shaderProgram->setUniform((_getStructMemberName() + "ambient").c_str(), ambientColor);
    shaderProgram->setUniform((_getStructMemberName() + "diffuse").c_str(), diffuseColor);
    shaderProgram->setUniform((_getStructMemberName() + "specular").c_str(), specularColor);
    shaderProgram->setUniform((_getStructMemberName() + "K_q").c_str(), K_q);

    shaderProgram->unbind();
}

std::string PointLight::_getStructMemberName()
{
    std::string temp = "u_pointLight[" + std::to_string(lightId) + "].";
    return temp;
}


// DirectionalLight

DirectionalLight::DirectionalLight(ShaderProgram *shaderProgram) :
    Light(shaderProgram)
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
