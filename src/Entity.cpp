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
    updateModelMatrix();
}

void Entity::rotate(glm::vec3 axis, float radians) {
    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, radians, axis);

    // Rotate orientation vector
    orientation = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(orientation, 1.0)));
    updateModelMatrix();
}

void Entity::setPosition(glm::vec3 position) {
    this->position = position;
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
    glm::vec3 const up(0.0f, 0.0f, 1.0f);
    //newModelMatrix = glm::rotate(newModelMatrix, , )

    // Scale
    newModelMatrix = glm::scale(newModelMatrix, glm::vec3(modelScale, modelScale, modelScale));

    modelMatrix = newModelMatrix;

}