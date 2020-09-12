#include "Light.h"

#include <string>

PointLight::PointLight(ShaderProgram *shaderProgram)
    : Light(shaderProgram) {

    isActive = false;

}

void PointLight::update() {

    std::string uName;

    shaderProgram->bind();
    uName = "u_pointLight[" + std::to_string(lightId) + "].isActive";
    shaderProgram->setUniform(uName.c_str(), isActive);

    uName = "u_pointLight[" + std::to_string(lightId) + "].position";
    shaderProgram->setUniform(uName.c_str(), position);

    uName = "u_pointLight[" + std::to_string(lightId) + "].ambient";
    shaderProgram->setUniform(uName.c_str(), ambientColor);

    uName = "u_pointLight[" + std::to_string(lightId) + "].diffuse";
    shaderProgram->setUniform(uName.c_str(), diffuseColor);

    uName = "u_pointLight[" + std::to_string(lightId) + "].specular";
    shaderProgram->setUniform(uName.c_str(), specularColor);

    uName = "u_pointLight[" + std::to_string(lightId) + "].K_c";
    shaderProgram->setUniform(uName.c_str(), K_c);

    uName = "u_pointLight[" + std::to_string(lightId) + "].K_l";
    shaderProgram->setUniform(uName.c_str(), K_l);

    uName = "u_pointLight[" + std::to_string(lightId) + "].K_q";
    shaderProgram->setUniform(uName.c_str(), K_q);
    shaderProgram->unbind();

}
