#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

Entity::Entity(Model *model, ShaderProgram *shaderProgram)
    : model(model), shaderProgram(shaderProgram) {
    
        // Empty...

}

void Entity::draw(glm::mat4 viewProjMatrix) {
    glm::mat4 modelViewProj = viewProjMatrix * modelMatrix;
    shaderProgram->setUniform("u_modelViewProj", modelViewProj);
    model->draw(shaderProgram);
}

void Entity::translate(glm::vec3 vector) {
    position += vector;
    modelMatrix = glm::translate(modelMatrix, vector);
}

void Entity::rotate(glm::vec3 axis, float radians) {
    modelMatrix = glm::rotate(modelMatrix, radians, axis);
}