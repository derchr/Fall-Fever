#include "World.h"

#include <iostream>

// At this moment, I don't have any idea how to implement the initializer list
// with an array of objects that don't have an default contructor. So the default
// constructors for Light and PointLight have only this reason to exist.

World::World(ShaderProgram *shaderProgram)
    : shaderProgram(shaderProgram) {

    for(unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
        pointLights[i].setId(i);
        pointLights[i].setShaderProgram(shaderProgram);
    }

}

void World::addEntity(Entity entity) {
    uint32_t new_id = entities.size();
    entity.setId(new_id);
    entities.push_back(entity);
}

void World::removeEntity(uint32_t id) {

    for(auto it = entities.begin(); it != entities.end(); it++) {
        if(it->getId() == id) {
            entities.erase(it);
        }
    }

    std::cout << "[Warning] Entity with ID " << id << " could not be removed." << std::endl;
}

void World::updateLight(unsigned int lightId, glm::vec3 position, glm::vec3 color) {
    pointLights[lightId].setActive(true);
    pointLights[lightId].setPosition(position);
    pointLights[lightId].setColor(color);
}

void World::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition) {

    for(auto it = entities.begin(); it != entities.end(); it++) {
        it->draw(viewProjMatrix, viewPosition);
    }

}
