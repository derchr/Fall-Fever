#include "Entity.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

uint32_t Entity::s_idCounter = 0;

Entity::Entity(const std::string &name, Model *model, ShaderProgram *shaderProgram)
    : m_uniqueName(name), m_model(model), m_shaderProgram(shaderProgram)
{
    m_id = s_idCounter++;
}

void Entity::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition)
{
    m_shaderProgram->bind();

    glm::mat4 modelViewProj = viewProjMatrix * m_modelMatrix;
    m_shaderProgram->setUniform("u_modelViewProjMatrix", modelViewProj);
    m_shaderProgram->setUniform("u_modelMatrix", m_modelMatrix);

    glm::mat3 normalMatrix = glm::mat3(m_modelMatrix);
    normalMatrix = glm::transpose(glm::inverse(normalMatrix));
    m_shaderProgram->setUniform("u_normalMatrix", normalMatrix);

    m_shaderProgram->setUniform("u_viewPosition", viewPosition);

    // Draw the model
    m_model->draw(m_shaderProgram);

    m_shaderProgram->unbind();
}

void Entity::drawDirectionalShadows(glm::mat4 viewProjMatrix, ShaderProgram *p_shaderProgram)
{
    p_shaderProgram->bind();

    glm::mat4 modelViewProj = viewProjMatrix * m_modelMatrix;
    p_shaderProgram->setUniform("u_modelViewProjMatrix", modelViewProj);

    // Draw the model
    m_model->drawWithoutTextures();

    p_shaderProgram->unbind();
}

void Entity::drawPointShadows(ShaderProgram *p_shaderProgram)
{
    p_shaderProgram->bind();

    p_shaderProgram->setUniform("u_modelMatrix", m_modelMatrix);

    // Draw the model
    m_model->drawWithoutTextures();

    p_shaderProgram->unbind();
}

void Entity::translate(glm::vec3 vector)
{
    m_position += vector;
    updateModelMatrix();
}

void Entity::rotate(glm::vec3 axis, float radians)
{
    glm::quat rotation = glm::angleAxis(radians, axis);
    m_quaternion = rotation * m_quaternion;
    updateModelMatrix();
}

void Entity::setPosition(glm::vec3 position)
{
    this->m_position = position;
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 eulerAngles)
{
    m_quaternion = glm::quat(eulerAngles);
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 axis, float radians)
{
    m_quaternion = glm::angleAxis(radians, axis);
    updateModelMatrix();
}

void Entity::setScale(float scaleFactor)
{
    m_modelScale = scaleFactor;
    updateModelMatrix();
}

void Entity::updateModelMatrix()
{
    // Translate * Rotate * Scale * vertex_vec;
    // First scaling, then rotation, then translation

    // Translate
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);

    // Rotate
    glm::mat4 rotationMatrix = glm::toMat4(m_quaternion);

    // Scale
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(m_modelScale, m_modelScale, m_modelScale));

    m_modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

void Entity::setIsLightSource(bool temp)
{
    m_isLightSource = temp;
}

void Entity::setId(uint32_t id)
{
    this->m_id = id;
}

uint32_t Entity::getId()
{
    return m_id;
}

std::string Entity::getUniqueName()
{
    return m_uniqueName;
}

glm::vec3 Entity::getPosition()
{
    return m_position;
}

glm::mat4 Entity::getModelMatrix()
{
    return m_modelMatrix;
}

bool Entity::getIsLightSource()
{
    return m_isLightSource;
}

Skybox::Skybox(Model *cubeModel, ShaderProgram *shaderProgram, const char *texturePseudoPath)
    : m_cubeModel(cubeModel), m_shaderProgram(shaderProgram), m_cubeMap(texturePseudoPath),
      m_vertexArray(cubeModel->getMesh(0)->getVertexArray())
{
    // Empty
}

void Skybox::draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    // To disable face culling first get current state
    GLboolean active;
    glGetBooleanv(GL_CULL_FACE_MODE, &active);
    glDisable(GL_CULL_FACE);

    glDepthMask(GL_FALSE);
    m_shaderProgram->bind();

    // Delete any translation from the skybox cube
    glm::mat4 viewProjectionMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));

    m_shaderProgram->setUniform("u_viewProjectionMatrix", viewProjectionMatrix);

    m_cubeMap.bind(m_shaderProgram);
    m_cubeModel->getMesh(0)->drawWithoutTextures();
    m_cubeMap.unbind();

    m_shaderProgram->unbind();
    glDepthMask(GL_TRUE);

    // Restore face culling
    if (active) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
