#pragma once

#include "FrameBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

class Camera;
class ModelEntity;
class Light;
class PointLight;
class DirectionalLight;
class ShaderProgram;
class Skybox;
class Model;

class Scene
{
public:
    Scene(std::vector<ShaderProgram *> shaderPrograms);
    ~Scene();

    void addEntity(ModelEntity *entity);
    void removeEntityByName(const std::string &name);
    void clearEntities();

    void updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color, float intensity);
    void updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color);

    std::vector<ModelEntity *> getEntities();
    std::vector<PointLight *> getPointLights();
    DirectionalLight *getDirectionalLight();
    Skybox *getSkybox();
    ModelEntity *getEntityByName(const std::string &name);
    ModelEntity *getEntityById(uint32_t id);
    Model *getModelByName(const std::string &name);

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);
    void calculateShadows(ShaderProgram *directionalShaderProgram, ShaderProgram *pointShaderProgram);

private:
    ShaderProgram *m_shaderProgram;

    std::vector<Model *> m_models;
    std::vector<ModelEntity *> m_entities;
    Skybox *m_skybox;

    // Lights
    std::vector<Light *> m_lights;

    // Shadows
    const int SHADOW_RES = 4096 / 4;
    DepthMap m_depthMapDirectionalFBO;
    std::vector<DepthMapCube *> m_depthMapPointFBO;
    // Shadow projection matrices
    const float m_nearPlaneDirectional = 1.0f;
    const float m_farPlaneDirectional = 15.0f;
    glm::mat4 m_directionalLightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_nearPlaneDirectional, m_farPlaneDirectional);
    const float m_aspectRatioPoint = 1.0f;
    const float m_nearPlanePoint = 1.0f;
    const float m_farPlanePoint = 25.0f;
    glm::mat4 m_pointLightProjection =
        glm::perspective(glm::radians(90.0f), m_aspectRatioPoint, m_nearPlanePoint, m_farPlanePoint);
};
