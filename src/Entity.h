#pragma once

#include "Model.h"
#include "Texture.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Entity {

public:

    Entity(Model *model, ShaderProgram *shaderProgram);
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

    void setIsLightSource(bool temp) { isLightSource = temp;}

    void setId(uint32_t id) { this->id = id; }
    uint32_t getId() { return id; }

    glm::vec3 getPosition() { return position; }
    glm::mat4 getModelMatrix() { return modelMatrix; }
    bool getIsLightSource() { return isLightSource; }

private:

    void updateModelMatrix();

    uint32_t id;
    bool isLightSource = false;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat quaternion;
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);


    float modelScale = 1.0f;

    Model *model;

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    ShaderProgram *shaderProgram;

};

class Skybox {

public:

    Skybox(Model *cubeModel, ShaderProgram *shaderProgram, const char *texturePseudoPath);
    ~Skybox() = default;

    void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:

    Model *cubeModel;
    ShaderProgram *shaderProgram;

    CubeMap cubeMap;

    VertexArray *vertexArray;

};
