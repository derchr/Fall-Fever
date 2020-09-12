#pragma once

#include <vector>

#include "Light.h"
#include "Camera.h"
#include "Entity.h"

class Scene {

public:

    Scene(ShaderProgram *shaderProgram);
    ~Scene() = default;

    void addEntity(Entity entity);
    void removeEntity(uint32_t id);

    std::vector<Entity> getEntities() { return entities; }

    void drawScene(glm::mat4 viewProjMatrix, glm::vec3 viewPosition);

private:

    ShaderProgram *shaderProgram;

    std::vector<Entity> entities;

    //DirectionalLight directionalLight;
    PointLight pointLights[NUM_POINT_LIGHTS];
    //SpotLight spotLight;

};