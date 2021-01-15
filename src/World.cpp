#include "World.h"
#include "JsonParser.h"
#include "Controller.h"

#include <iostream>

World::World(std::vector<ShaderProgram*> shaderPrograms) :
    shaderProgram(Controller::getShaderProgramByName(shaderPrograms, "defaultProgram")),
    directionalLight(shaderProgram),
    depthMapDirectionalFBO(DEPTHMAP_NORMAL, SHADOW_RES)
{
    // PointLights
    for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
        PointLight new_pointLight(shaderProgram);
        new_pointLight.setId(i);
        pointLights.push_back(new_pointLight);
    }

    // Create 4 depthMaps
    for (int i = 0; i < 4; i++) {
        DepthMap *temp_depthMap = new DepthMap(DEPTHMAP_CUBEMAP, SHADOW_RES);
        depthMapPointFBO.push_back(temp_depthMap);
    }

    // This will be removed in future when gloss maps are implemented
    shaderProgram->bind();
    shaderProgram->setUniform("u_material.shininess", 100.0f);
    shaderProgram->unbind();

    JsonParser worldParser("res/models.json");
    models = worldParser.getModels();
    entities = worldParser.getEntities(models, shaderPrograms);
    skybox = worldParser.getSkybox(getModelByName("cube"), Controller::getShaderProgramByName(shaderPrograms, "skyboxProgram"));
}

World::~World()
{
    // Iterate over depthMapPointFBO vector and delete all items
    for (auto it = depthMapPointFBO.begin(); it != depthMapPointFBO.end(); it++) {
        delete (*it);
    }
    // Iterate over models and entities and delete all items
    for (auto it = models.begin(); it != models.end(); it++) {
        delete (*it);
    }
    for (auto it = entities.begin(); it != entities.end(); it++) {
        delete (*it);
    }

    delete skybox;
}

void World::addEntity(Entity *entity)
{
    entities.push_back(entity);
}

void World::removeEntityByName(std::string name)
{
    for (auto it = entities.begin(); it != entities.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            entities.erase(it);
            return;
        }
    }

    std::cout << "[Warning] Entity with name " << name << " could not be removed." << std::endl;
}

void World::clearEntities()
{
    for (auto it = entities.begin(); it != entities.end(); it++) {
        entities.erase(it);
    }
}

void World::updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color)
{
    pointLights[lightId].setActive(active);
    pointLights[lightId].setPosition(position);
    pointLights[lightId].setColor(color);
}

void World::updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color)
{
    directionalLight.setActive(active);
    directionalLight.setDirection(direction);
    directionalLight.setDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
    directionalLight.setColor(color);
}

void World::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition)
{
    // Draw all entities
    for (auto it = entities.begin(); it != entities.end(); it++) {
        (*it)->draw(viewProjMatrix, viewPosition);
    }

    // Calculate shadows (unclear if it should belong here or somewhere else)
    // calculateShadows();
}

void World::calculateShadows(ShaderProgram *directionalShaderProgram, ShaderProgram *pointShaderProgram)
{
    // Get old viewport dimensions to reset them later...
    GLint VIEWPORT[4];
    glGetIntegerv(GL_VIEWPORT, VIEWPORT);

    glViewport(0, 0, SHADOW_RES, SHADOW_RES);
    // Switch face culling (Peter panning)
    glCullFace(GL_BACK);

    depthMapDirectionalFBO.bind();

    glClear(GL_DEPTH_BUFFER_BIT);

    // --- Directional shadows ---
    glm::mat4 directionalLightView = glm::lookAt(-5.0f * glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f));
    glm::mat4 directionalLightViewProjectionMatrix = directionalLightProjection * directionalLightView;

    // Draw scene from light perspective
    // Draw all entities
    for (auto it = entities.begin(); it != entities.end(); it++) {
        (*it)->drawDirectionalShadows(directionalLightViewProjectionMatrix, directionalShaderProgram);
    }

    depthMapDirectionalFBO.unbind();

    shaderProgram->bind();

    // Send lightViewProjMatrix to basic shader
    shaderProgram->setUniform("u_directionalLightViewProjMatrix", directionalLightViewProjectionMatrix);

    // Send shadowMap to basic shader
    int textureUnit = TEXTURE_TYPE_NUM_ITEMS * 2;
    shaderProgram->setUniform("u_texture_directionalShadowMap", (int)textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMapDirectionalFBO.getDepthMap());

    shaderProgram->unbind();

    // --- Point shadows ---

    // 4 depthMaps for 4 point lights
    for (int i = 0; i < 1; i++) {
        depthMapPointFBO[i]->bind();

        glClear(GL_DEPTH_BUFFER_BIT);

        // Create 6 view matrices for every face of the cubeMap
        std::vector<glm::mat4> viewProjMatrices;
        glm::vec3 lightPos = pointLights[i].getPosition();
        viewProjMatrices.push_back(pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        viewProjMatrices.push_back(pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        viewProjMatrices.push_back(pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        viewProjMatrices.push_back(pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        viewProjMatrices.push_back(pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        viewProjMatrices.push_back(pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        pointShaderProgram->bind();

        for (int i = 0; i < 6; i++) {
            pointShaderProgram->setUniform(("u_shadowMatrices[" + std::to_string(i) + "]").c_str(), viewProjMatrices[i]);
        }

        pointShaderProgram->setUniform("pointShadowDepthMapFarPlane", far_plane_point);
        pointShaderProgram->setUniform("v_lightPos", lightPos);

        // Draw scene from light perspective
        // Draw all entities
        for (auto it = entities.begin(); it != entities.end(); it++) {
            (*it)->drawPointShadows(pointShaderProgram);
        }

        depthMapPointFBO[i]->unbind();

        shaderProgram->bind();

        shaderProgram->setUniform("pointShadowDepthMapFarPlane", far_plane_point);

        textureUnit = TEXTURE_TYPE_NUM_ITEMS * 2 + i + 1;
        shaderProgram->setUniform("u_texture_pointShadowMap0", (int)textureUnit);
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapPointFBO[i]->getCubeMapId());

        shaderProgram->unbind();
    }

    // Reset viewport size
    glViewport(VIEWPORT[0], VIEWPORT[1], VIEWPORT[2], VIEWPORT[3]);
    glCullFace(GL_FRONT);
}

Model* World::getModelByName(std::string name)
{
    for (auto it = models.begin(); it != models.end(); it++) {
        if((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] Model could not be found by unique name \"" << name << "\"" << std::endl;
    return nullptr;
}

Entity* World::getEntityByName(std::string name)
{
    for (auto it = entities.begin(); it != entities.end(); it++) {
        if((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] Entity could not be found by unique name \"" << name << "\"" << std::endl;
    return nullptr;
}

Entity* World::getEntityById(uint32_t id)
{
    for (auto it = entities.begin(); it != entities.end(); it++) {
        if((*it)->getId() == id) {
            return *it;
        }
    }
    std::cout << "[Warning] Entity could not be found by ID \"" << id << "\"" << std::endl;
    return nullptr;
}

PointLight* World::getPointLights()
{
    return pointLights.data();
}

std::vector< Entity* > World::getEntities()
{
    return entities;
}

Skybox* World::getSkybox()
{
    return skybox;
}
