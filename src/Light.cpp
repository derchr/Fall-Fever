#include "Light.h"

#include <string>

// PointLight

PointLight::PointLight(ShaderProgram *shaderProgram)
    : Light(shaderProgram) {

}

void PointLight::update() {

    // Only temp
    glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);
    glm::vec3 specularColor = lightColor * glm::vec3(1.0f);

    shaderProgram->bind();

    shaderProgram->setUniform((_getStructMemberName() + "isActive").c_str(), isActive);
    shaderProgram->setUniform((_getStructMemberName() + "position").c_str(), position);
    shaderProgram->setUniform((_getStructMemberName() + "ambient").c_str(), ambientColor);
    shaderProgram->setUniform((_getStructMemberName() + "diffuse").c_str(), diffuseColor);
    shaderProgram->setUniform((_getStructMemberName() + "specular").c_str(), specularColor);
    shaderProgram->setUniform((_getStructMemberName() + "K_c").c_str(), K_c);
    shaderProgram->setUniform((_getStructMemberName() + "K_l").c_str(), K_l);
    shaderProgram->setUniform((_getStructMemberName() + "K_q").c_str(), K_q);

    shaderProgram->unbind();
}

std::string PointLight::_getStructMemberName() {
    std::string temp = "u_pointLight[" + std::to_string(lightId) + "].";
    return temp;
}


// DirectionalLight

DirectionalLight::DirectionalLight(ShaderProgram *shaderProgram)
    : Light(shaderProgram) {


}

void DirectionalLight::update() {

    // Only temp
    glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);
    glm::vec3 specularColor = lightColor * glm::vec3(1.0f);

    shaderProgram->bind();

    shaderProgram->setUniform("u_directionalLight.isActive", isActive);
    shaderProgram->setUniform("u_directionalLight.direction", direction);
    shaderProgram->setUniform("u_directionalLight.ambient", ambientColor * 0.25f);
    shaderProgram->setUniform("u_directionalLight.diffuse", diffuseColor * 0.25f);
    shaderProgram->setUniform("u_directionalLight.specular", specularColor * 0.25f);

    shaderProgram->unbind();

}
