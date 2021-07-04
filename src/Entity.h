#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Entity
{
public:
    Entity(const std::string &name, Model *model, ShaderProgram *shaderProgram);
    ~Entity() = default;

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);
    void drawDirectionalShadows(glm::mat4 viewProjMatrix, ShaderProgram *p_shaderProgram);
    void drawPointShadows(ShaderProgram *p_shaderProgram);

    void translate(glm::vec3 vector);
    void rotate(glm::vec3 axis, float radians);

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 eulerAngles);
    void setRotation(glm::vec3 axis, float radians);
    void setScale(float scaleFactor);
    void setIsLightSource(bool temp);
    void setId(uint32_t id);

    uint32_t getId();
    std::string getUniqueName();
    glm::vec3 getPosition();
    glm::mat4 getModelMatrix();
    bool getIsLightSource();

private:
    void updateModelMatrix();

    uint32_t m_id;
    static uint32_t s_idCounter;
    std::string m_uniqueName;

    Model *m_model;
    ShaderProgram *m_shaderProgram;

    bool m_isLightSource = false;

    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat m_quaternion;

    float m_modelScale = 1.0f;
};

class Skybox
{
public:
    Skybox(Model *cubeModel, ShaderProgram *shaderProgram, const char *texturePseudoPath);
    ~Skybox() = default;

    void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:
    Model *m_cubeModel;
    ShaderProgram *m_shaderProgram;

    CubeMap m_cubeMap;

    VertexArray *m_vertexArray;
};
