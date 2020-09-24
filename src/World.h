#pragma once

#include <vector>

#include "Light.h"
#include "Camera.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "Framebuffer.h"

class World {

public:

    World(ShaderProgram *shaderProgram);
    ~World() = default;

    void addEntity(Entity entity);
    void removeEntity(uint32_t id);

    void updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color);
    void updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color);

    std::vector<Entity> * getEntities() { return &entities; }

    PointLight * getPointLights() { return pointLights; }

    void draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);
    void calculateShadows(ShaderProgram *shaderProgram);

private:

    ShaderProgram *shaderProgram;

    std::vector<Entity> entities;

    // Lights
    DirectionalLight directionalLight;
    PointLight pointLights[NUM_POINT_LIGHTS];
    //SpotLight spotLight;

    // Shadows
    const int SHADOW_RES = 1024;
    DepthMap depthMapFBO;

};
