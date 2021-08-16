#include "Entity.h"
#include "Mesh.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "VertexArray.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

uint32_t Entity::s_idCounter = 0;

Entity::Entity(const std::string &name) : m_id(s_idCounter++), m_uniqueName(name)
{}

Entity::~Entity()
{}

uint32_t Entity::getId() const
{
    return m_id;
}

const std::string &Entity::getUniqueName() const
{
    return m_uniqueName;
}

void Entity::setParent(Entity *parent)
{
    m_parent = parent;
}

void Entity::addChild(Entity *child)
{
    m_children.push_back(child);
}

void Entity::translate(glm::vec3 vector)
{
    for (auto &child : m_children) {
        child->translate(vector);
    }

    m_position += vector;

    updateModelMatrix();
}

void Entity::rotate(glm::vec3 axis, float radians)
{
    for (auto &child : m_children) {
        child->rotate(axis, radians);
    }

    glm::quat rotation = glm::angleAxis(radians, axis);
    m_quaternion = rotation * m_quaternion;

    updateModelMatrix();
}

void Entity::setPosition(glm::vec3 position)
{
    for (auto &child : m_children) {
        child->setPosition(child->getPosition() - m_position + position);
    }

    m_position = position;
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 eulerAngles)
{
    for (auto &child : m_children) {
        child->setRotation(eulerAngles);
    }

    m_quaternion = glm::quat(eulerAngles);
    updateModelMatrix();
}

void Entity::setRotation(glm::vec3 axis, float radians)
{
    for (auto &child : m_children) {
        child->setRotation(axis, radians);
    }

    m_quaternion = glm::angleAxis(radians, axis);
    updateModelMatrix();
}

void Entity::setScale(float scale)
{
    for (auto &child : m_children) {
        child->setScale(scale);
    }

    m_scale = scale;
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
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(m_scale, m_scale, m_scale));

    m_modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

glm::vec3 Entity::getPosition() const
{
    return m_position;
}

glm::mat4 Entity::getModelMatrix() const
{
    return m_modelMatrix;
}

ModelEntity::ModelEntity(Prototype prototype, Model *model, ShaderProgram *shaderProgram)
    : Entity(prototype.name), m_model(model), m_shaderProgram(shaderProgram)
{
    setPosition(prototype.position);
    setRotation(prototype.rotation);
    setScale(prototype.scale);
}

void ModelEntity::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition)
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

void ModelEntity::drawDirectionalShadows(glm::mat4 viewProjMatrix, ShaderProgram *shaderProgram)
{
    shaderProgram->bind();

    glm::mat4 modelViewProj = viewProjMatrix * m_modelMatrix;
    shaderProgram->setUniform("u_modelViewProjMatrix", modelViewProj);

    // Draw the model
    m_model->drawWithoutTextures();

    shaderProgram->unbind();
}

void ModelEntity::drawPointShadows(ShaderProgram *shaderProgram)
{
    shaderProgram->bind();

    shaderProgram->setUniform("u_modelMatrix", m_modelMatrix);

    // Draw the model
    m_model->drawWithoutTextures();

    shaderProgram->unbind();
}

Skybox::Skybox(Prototype prototype, Model *cubeModel, ShaderProgram *shaderProgram)
    : m_cubeModel(cubeModel), m_shaderProgram(shaderProgram), m_cubeMap(new CubeMap(prototype.texturePath.c_str())),
      m_vertexArray(cubeModel->getMesh(0)->getVertexArray())
{}

Skybox::~Skybox()
{
    delete m_cubeMap;
}

void Skybox::initializeOnGPU()
{
    m_cubeMap->initializeOnGPU();
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

    m_cubeMap->bind(m_shaderProgram);
    m_cubeModel->getMesh(0)->drawWithoutTextures();
    m_cubeMap->unbind();

    m_shaderProgram->unbind();
    glDepthMask(GL_TRUE);

    // Restore face culling
    if (active) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
