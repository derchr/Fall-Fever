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

    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    shaderProgram->setUniform("u_normalMatrix", normalMatrix);

    shaderProgram->setUniform("u_viewPosition", viewPosition);

    // Draw the model
    model->draw(shaderProgram);

    shaderProgram->unbind();
}

void Entity::translate(glm::vec3 vector) {
    position += vector;
    modelMatrix = glm::translate(modelMatrix, vector);
}

void Entity::rotate(glm::vec3 axis, float radians) {
    modelMatrix = glm::rotate(modelMatrix, radians, axis);
}

void Entity::scale(float scaleFactor) {
    modelScale = scaleFactor;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
}