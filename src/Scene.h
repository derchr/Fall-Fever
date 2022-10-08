#pragma once

#include "resources/Resource.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

class ModelEntity;
class Light;
class PointLight;
class DirectionalLight;
class ShaderProgram;
class Skybox;

class Scene
{
public:
    Scene(std::vector<std::shared_ptr<ShaderProgram>> shaderPrograms);

    void updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color, float intensity);
    void updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color);

    std::vector<std::shared_ptr<ModelEntity>> getEntities();
    std::vector<std::shared_ptr<PointLight>> getPointLights();
    std::shared_ptr<DirectionalLight> getDirectionalLight();
    std::shared_ptr<Skybox> getSkybox();
    std::shared_ptr<ModelEntity> getEntityByName(const std::string &name);
    std::shared_ptr<ModelEntity> getEntityById(uint32_t id);

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);

private:
    ShaderProgram &m_shaderProgram;

    std::vector<ResourceId> m_models;
    std::vector<std::shared_ptr<ModelEntity>> m_entities;
    std::shared_ptr<Skybox> m_skybox;

    // Lights
    std::vector<std::shared_ptr<Light>> m_lights;
};
