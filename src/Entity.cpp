#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>


Entity::Entity(Model *model, ShaderProgram *shaderProgram)
    : model(model), shaderProgram(shaderProgram) {
    
        // Empty...

}

void Entity::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition) {

    shaderProgram->bind();

    glm::mat4 modelViewProj = viewProjMatrix * modelMatrix;
    shaderProgram->setUniform("u_modelViewProjMatrix", modelViewProj);
    shaderProgram->setUniform("u_modelMatrix", modelMatrix);

    glm::mat3 normalMatrix = glm::mat3(modelMatrix);
    normalMatrix = glm::transpose(glm::inverse(normalMatrix));
    shaderProgram->setUniform("u_normalMatrix", normalMatrix);

    shaderProgram->setUniform("u_viewPosition", viewPosition);

    // Draw the model
    model->draw(shaderProgram);

    shaderProgram->unbind();

}

void Entity::translate(glm::vec3 vector) {
    position += vector;
    updateModelMatrix();
}

void Entity::rotate(glm::vec3 axis, float radians) {
    glm::quat rotation = glm::angleAxis(radians, axis);
    quaternion = rotation * quaternion;
    updateModelMatrix();
}

void Entity::setPosition(glm::vec3 position) {
    this->position = position;
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 eulerAngles) {
    quaternion = glm::quat(eulerAngles);
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 axis, float radians) {
    quaternion = glm::angleAxis(radians, axis);
    updateModelMatrix();
}

void Entity::setScale(float scaleFactor) {
    modelScale = scaleFactor;
    updateModelMatrix();
}

void Entity::updateModelMatrix() {
    
    // Translate * Rotate * Scale * vertex_vec;
    // First scaling, then rotation, then translation

    // Translate
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

    // Rotate
    glm::mat4 rotationMatrix = glm::toMat4(quaternion);

    // Scale
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale, modelScale, modelScale));

    modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

}