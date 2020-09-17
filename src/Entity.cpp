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
    orientation += axis * radians;
    updateModelMatrix();
}

void Entity::setPosition(glm::vec3 position) {
    this->position = position;
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 orientation) {
    this->orientation = orientation;
    updateModelMatrix();
}

void Entity::setScale(float scaleFactor) {
    modelScale = scaleFactor;
    updateModelMatrix();
}

void Entity::updateModelMatrix() {
    
    glm::mat4 newModelMatrix(1.0f);

    // Translate * Rotate * Scale * vertex_vec;
    // First scaling, then rotation, then translation

    // Translate
    newModelMatrix = glm::translate(newModelMatrix, position);

    // Rotate
    newModelMatrix = glm::rotate(newModelMatrix, orientation.x, glm::vec3(1.f, 0.f, 0.f));
    newModelMatrix = glm::rotate(newModelMatrix, orientation.y, glm::vec3(0.f, 1.f, 0.f));
    newModelMatrix = glm::rotate(newModelMatrix, orientation.z, glm::vec3(0.f, 0.f, 1.f));

    // Scale
    newModelMatrix = glm::scale(newModelMatrix, glm::vec3(modelScale, modelScale, modelScale));

    modelMatrix = newModelMatrix;

}