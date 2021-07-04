#include "World.h"
#include "Entity.h"
#include "Controller.h"
#include "Model.h"
#include "Light.h"
#include "JsonParser.h"
#include "Camera.h"
#include "Texture.h"
#include "ShaderProgram.h"

#include <iostream>

World::World(std::vector<ShaderProgram *> shaderPrograms)
    : m_shaderProgram(Controller::getShaderProgramByName(shaderPrograms, "defaultProgram")),
      m_depthMapDirectionalFBO(DepthMapType::Normal, SHADOW_RES)
{
    // Create 4 depthMaps
    for (int i = 0; i < 4; i++) {
        DepthMap *temp_depthMap = new DepthMap(DepthMapType::CubeMap, SHADOW_RES);
        m_depthMapPointFBO.push_back(temp_depthMap);
    }

    // This will be removed in future when gloss maps are implemented
    m_shaderProgram->bind();
    m_shaderProgram->setUniform("u_material.shininess", 100.0f);
    m_shaderProgram->unbind();

    JsonParser modelParser("data/models.json");
    m_models = modelParser.getModels();

    for (const auto &it : m_models) {
        it->prepareModel();
    }

    m_entities = modelParser.getEntities(m_models, shaderPrograms);
    m_skybox = modelParser.getSkybox(getModelByName("cube"),
                                     Controller::getShaderProgramByName(shaderPrograms, "skyboxProgram"));

    JsonParser lightParser("data/lights.json");
    m_lights = lightParser.getLights(m_shaderProgram);
}

World::~World()
{
    // Iterate over depthMapPointFBO vector and delete all items
    for (auto it = m_depthMapPointFBO.begin(); it != m_depthMapPointFBO.end(); it++) {
        delete (*it);
    }
    // Iterate over models and entities and delete all items
    for (auto it = m_models.begin(); it != m_models.end(); it++) {
        delete (*it);
    }
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        delete (*it);
    }

    delete m_skybox;
}

void World::addEntity(Entity *entity)
{
    m_entities.push_back(entity);
}

void World::removeEntityByName(std::string name)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            m_entities.erase(it);
            return;
        }
    }

    std::cout << "[Warning] Entity with name " << name << " could not be removed." << std::endl;
}

void World::clearEntities()
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        m_entities.erase(it);
    }
}

void World::updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color, float intensity)
{
    std::vector<PointLight *> pointLights = getPointLights();
    pointLights[lightId]->setActive(active);
    pointLights[lightId]->setPosition(position);
    pointLights[lightId]->setIntensity(intensity);
    pointLights[lightId]->setColor(color);
}

void World::updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color)
{
    DirectionalLight *directionalLight = getDirectionalLight();
    directionalLight->setActive(active);
    directionalLight->setDirection(direction);
    directionalLight->setColor(color);
}

void World::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition)
{
    // Draw all entities
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
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

    m_depthMapDirectionalFBO.bind();

    glClear(GL_DEPTH_BUFFER_BIT);

    // --- Directional shadows ---
    glm::mat4 directionalLightView =
        glm::lookAt(-5.0f * glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 directionalLightViewProjectionMatrix = m_directionalLightProjection * directionalLightView;

    // Draw scene from light perspective
    // Draw all entities
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        (*it)->drawDirectionalShadows(directionalLightViewProjectionMatrix, directionalShaderProgram);
    }

    m_depthMapDirectionalFBO.unbind();

    m_shaderProgram->bind();

    // Send lightViewProjMatrix to basic shader
    m_shaderProgram->setUniform("u_directionalLightViewProjMatrix", directionalLightViewProjectionMatrix);

    // Send shadowMap to basic shader
    int textureUnit = static_cast<int>(TextureType::TEXTURE_TYPE_NUM_ITEMS) * 2;
    m_shaderProgram->setUniform("u_texture_directionalShadowMap", (int)textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_depthMapDirectionalFBO.getDepthMap());

    m_shaderProgram->unbind();

    // --- Point shadows ---
    std::vector<PointLight *> pointLights = getPointLights();

    // 4 depthMaps for 4 point lights
    for (int i = 0; i < 1; i++) {
        m_depthMapPointFBO[i]->bind();

        glClear(GL_DEPTH_BUFFER_BIT);

        // Create 6 view matrices for every face of the cubeMap
        std::vector<glm::mat4> viewProjMatrices;
        glm::vec3 lightPos = pointLights[i]->getPosition();
        viewProjMatrices.push_back(m_pointLightProjection * glm::lookAt(lightPos,
                                                                        lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                                                                        glm::vec3(0.0f, -1.0f, 0.0f)));
        viewProjMatrices.push_back(m_pointLightProjection * glm::lookAt(lightPos,
                                                                        lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                                        glm::vec3(0.0f, -1.0f, 0.0f)));
        viewProjMatrices.push_back(m_pointLightProjection * glm::lookAt(lightPos,
                                                                        lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                                                                        glm::vec3(0.0f, 0.0f, 1.0f)));
        viewProjMatrices.push_back(m_pointLightProjection * glm::lookAt(lightPos,
                                                                        lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                                                                        glm::vec3(0.0f, 0.0f, -1.0f)));
        viewProjMatrices.push_back(m_pointLightProjection * glm::lookAt(lightPos,
                                                                        lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                                                                        glm::vec3(0.0f, -1.0f, 0.0f)));
        viewProjMatrices.push_back(m_pointLightProjection * glm::lookAt(lightPos,
                                                                        lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                                                                        glm::vec3(0.0f, -1.0f, 0.0f)));

        pointShaderProgram->bind();

        for (int i = 0; i < 6; i++) {
            pointShaderProgram->setUniform(("u_shadowMatrices[" + std::to_string(i) + "]").c_str(),
                                           viewProjMatrices[i]);
        }

        pointShaderProgram->setUniform("pointShadowDepthMapFarPlane", m_farPlanePoint);
        pointShaderProgram->setUniform("v_lightPos", lightPos);

        // Draw scene from light perspective
        // Draw all entities
        for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
            (*it)->drawPointShadows(pointShaderProgram);
        }

        m_depthMapPointFBO[i]->unbind();

        m_shaderProgram->bind();

        m_shaderProgram->setUniform("pointShadowDepthMapFarPlane", m_farPlanePoint);

        textureUnit = static_cast<int>(TextureType::TEXTURE_TYPE_NUM_ITEMS) * 2 + i + 1;
        m_shaderProgram->setUniform("u_texture_pointShadowMap0", (int)textureUnit);
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMapPointFBO[i]->getCubeMapId());

        m_shaderProgram->unbind();
    }

    // Reset viewport size
    glViewport(VIEWPORT[0], VIEWPORT[1], VIEWPORT[2], VIEWPORT[3]);
    glCullFace(GL_FRONT);
}

Model *World::getModelByName(std::string name)
{
    for (auto it = m_models.begin(); it != m_models.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] Model could not be found by unique name \"" << name << "\"" << std::endl;
    return nullptr;
}

Entity *World::getEntityByName(std::string name)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    std::cout << "[Warning] Entity could not be found by unique name \"" << name << "\"" << std::endl;
    return nullptr;
}

Entity *World::getEntityById(uint32_t id)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getId() == id) {
            return *it;
        }
    }
    std::cout << "[Warning] Entity could not be found by ID \"" << id << "\"" << std::endl;
    return nullptr;
}

std::vector<PointLight *> World::getPointLights()
{
    std::vector<PointLight *> temp_pointLights;

    for (auto it = m_lights.begin(); it != m_lights.end(); it++) {
        PointLight *temp_pointLight = dynamic_cast<PointLight *>(*it);
        if (temp_pointLight) {
            temp_pointLights.push_back(temp_pointLight);
        }
    }

    return temp_pointLights;
}

DirectionalLight *World::getDirectionalLight()
{
    DirectionalLight *temp_directionalLight = nullptr;

    for (auto it = m_lights.begin(); it != m_lights.end(); it++) {
        temp_directionalLight = dynamic_cast<DirectionalLight *>(*it);
        if (temp_directionalLight)
            break;
    }

    return temp_directionalLight;
}

std::vector<Entity *> World::getEntities()
{
    return m_entities;
}

Skybox *World::getSkybox()
{
    return m_skybox;
}
