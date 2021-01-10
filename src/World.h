#pragma once

#include <vector>

#include "Light.h"
#include "Camera.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "Framebuffer.h"

class World
{
public:
    World(std::vector<ShaderProgram*> shaderPrograms);
    ~World();

    void addEntity(Entity *entity);
    void removeEntityByName(std::string name);
    void clearEntities();

    void loadWorld(unsigned int id);

    void updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color);
    void updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color);

    std::vector<Entity*> getEntities()
    {
        return entities;
    }

    PointLight *getPointLights()
    {
        return pointLights.data();
    }

    Entity* getEntityByName(std::string name);
    Entity* getEntityById(uint32_t id);
    Model* getModelByName(std::string name);

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);
    void calculateShadows(ShaderProgram *directionalShaderProgram, ShaderProgram *pointShaderProgram);

private:
    ShaderProgram *shaderProgram;

    std::vector<Model*> models;
    std::vector<Entity*> entities;

    // Lights
    DirectionalLight directionalLight;
    std::vector<PointLight> pointLights;
    //SpotLight spotLight;

    // Shadows
    const int SHADOW_RES = 4096 / 4;
    DepthMap depthMapDirectionalFBO;
    std::vector<DepthMap *> depthMapPointFBO;
    // Shadow projection matrices
    float near_plane = 1.0f, far_plane = 15.0f;
    glm::mat4 directionalLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    float aspect = 1.0f, near = 1.0f, far = 25.0f;
    glm::mat4 pointLightProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);
};
