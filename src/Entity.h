#pragma once

#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

class VertexArray;
class ShaderProgram;
class Model;

class Entity
{
public:
    struct Prototype
    {
        Prototype(const std::string &name, glm::vec3 position, glm::vec3 rotation, float scale)
            : name(name), position(position), rotation(rotation), scale(scale)
        {}
        virtual ~Prototype() = default;

        std::string name;
        glm::vec3 position;
        glm::vec3 rotation;
        float scale;
    };

    Entity(const std::string &name);
    virtual ~Entity() = 0;

    uint32_t getId() const;
    const std::string &getUniqueName() const;

    void translate(glm::vec3 vector);
    void rotate(glm::vec3 axis, float radians);

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 eulerAngles);
    void setRotation(glm::vec3 axis, float radians);
    void setScale(float scale);

    glm::vec3 getPosition() const;
    glm::mat4 getModelMatrix() const;

protected:
    void updateModelMatrix();

    const uint32_t m_id;
    static uint32_t s_idCounter;

    // TODO
    std::weak_ptr<Entity> m_parent;
    std::vector<std::shared_ptr<Entity>> m_children;

    std::string m_uniqueName;

    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat m_quaternion;
    float m_scale = 1.0f;
};

class ModelEntity : public Entity
{
public:
    struct Prototype : public Entity::Prototype
    {
        Prototype(const std::string &name, glm::vec3 position, glm::vec3 rotation, float scale,
                  const std::string &modelName, const std::string &shaderProgramName)
            : Entity::Prototype(name, position, rotation, scale), modelName(modelName),
              shaderProgramName(shaderProgramName)
        {}
        std::string modelName;
        std::string shaderProgramName;
    };

    ModelEntity(Prototype prototype, Model *model, ShaderProgram *shaderProgram);
    ~ModelEntity() = default;

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);
    void drawDirectionalShadows(glm::mat4 viewProjMatrix, ShaderProgram *p_shaderProgram);
    void drawPointShadows(ShaderProgram *p_shaderProgram);

private:
    Model *m_model;
    ShaderProgram *m_shaderProgram;
};

class Skybox
{
public:
    struct Prototype
    {
        std::string texturePath;
    };

    Skybox(Prototype prototype, Model *cubeModel, ShaderProgram *shaderProgram);
    ~Skybox();

    void initializeOnGPU();

    void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:
    bool m_isInitialized = false;

    Model *m_cubeModel;
    ShaderProgram *m_shaderProgram;

    CubeMap *m_cubeMap;

    VertexArray *m_vertexArray;
};
