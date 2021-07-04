#pragma once

#include <vector>

#include "Camera.h"
#include "Entity.h"
#include "Framebuffer.h"
#include "Light.h"
#include "ShaderProgram.h"

class World
{
public:
    World(std::vector<ShaderProgram *> shaderPrograms);
    ~World();

    void addEntity(Entity *entity);
    void removeEntityByName(std::string name);
    void clearEntities();

    void updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color, float intensity);
    void updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color);

    std::vector<Entity *> getEntities();
    std::vector<PointLight *> getPointLights();
    DirectionalLight *getDirectionalLight();
    Skybox *getSkybox();
    Entity *getEntityByName(std::string name);
    Entity *getEntityById(uint32_t id);
    Model *getModelByName(std::string name);

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);
    void calculateShadows(ShaderProgram *directionalShaderProgram, ShaderProgram *pointShaderProgram);

private:
    ShaderProgram *shaderProgram;

    std::vector<Model *> models;
    std::vector<Entity *> entities;
    Skybox *skybox;

    // Lights
    std::vector<Light *> lights;

    // Shadows
    const int SHADOW_RES = 4096 / 4;
    DepthMap depthMapDirectionalFBO;
    std::vector<DepthMap *> depthMapPointFBO;
    // Shadow projection matrices
    const float near_plane_directional = 1.0f;
    const float far_plane_directional = 15.0f;
    glm::mat4 directionalLightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane_directional, far_plane_directional);
    const float aspect_ratio_point = 1.0f;
    const float near_plane_point = 1.0f;
    const float far_plane_point = 25.0f;
    glm::mat4 pointLightProjection =
        glm::perspective(glm::radians(90.0f), aspect_ratio_point, near_plane_point, far_plane_point);
};
