#include "World.h"

#include <iostream>

// At this moment, I don't have any idea how to implement the initializer list
// with an array of objects that don't have an default contructor. So the default
// constructors for Light and PointLight have only this reason to exist.

World::World(ShaderProgram *shaderProgram)
    : shaderProgram(shaderProgram), depthMapFBO(SHADOW_RES) {

    // PointLights
    for(unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
        pointLights[i].setId(i);
        pointLights[i].setShaderProgram(shaderProgram);
    }

    // DirectionalLight
    directionalLight.setShaderProgram(shaderProgram);

    // This will be removed in future when gloss maps are implemented
    shaderProgram->bind();
    shaderProgram->setUniform("u_material.shininess", 100.0f);
    shaderProgram->unbind();

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

void World::updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color) {
    pointLights[lightId].setActive(active);
    pointLights[lightId].setPosition(position);
    pointLights[lightId].setColor(color);
}

void World::updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color) {
    directionalLight.setActive(active);
    directionalLight.setDirection(direction);
    directionalLight.setDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
    directionalLight.setColor(color);
}

void World::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition) {

    // Draw all entities
    for(auto it = entities.begin(); it != entities.end(); it++) {
        it->draw(viewProjMatrix, viewPosition);
    }

    // Calculate shadows
    // calculateShadows();

}

void World::calculateShadows(ShaderProgram *p_shaderProgram) {

    // Get old viewport dimensions to reset them later...
    GLint VIEWPORT[4];
    glGetIntegerv(GL_VIEWPORT, VIEWPORT);

    glViewport(0, 0, SHADOW_RES, SHADOW_RES);

    depthMapFBO.bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    // ConfigureShaderAndMatrices();
    float near_plane = 1.0f, far_plane = 15.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(-5.0f * glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3( 0.0f, 0.0f,  0.0f), glm::vec3( 0.0f, 1.0f,  0.0f));
    glm::mat4 lightViewProjectionMatrix = lightProjection * lightView;

    // Switch face culling (Peter panning)
    glCullFace(GL_BACK);

    // Draw scene from light perspective
    // Draw all entities
    for(auto it = entities.begin(); it != entities.end(); it++) {
        it->drawShadows(lightViewProjectionMatrix, p_shaderProgram);
    }

    glCullFace(GL_FRONT);

    depthMapFBO.unbind();

    // Reset viewport size
    glViewport(VIEWPORT[0], VIEWPORT[1], VIEWPORT[2], VIEWPORT[3]);

    shaderProgram->bind();

    // Send lightViewProjMatrix to basic shader
    shaderProgram->setUniform("u_lightViewProjMatrix", lightViewProjectionMatrix);

    // Send shadowMap to basic shader
    const int textureUnit = TEXTURE_TYPE_NUM_ITEMS * 2;
    shaderProgram->setUniform("u_texture_shadowMap", (int)textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMapFBO.getDepthMap());

    shaderProgram->unbind();

}
