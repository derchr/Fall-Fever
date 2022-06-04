#include "Scene.h"
#include "Camera.h"
#include "Controller.h"
#include "Entity.h"
#include "FrameBuffer.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "resources/Model.h"
#include "resources/ResourceHandler.h"
#include "resources/Texture.h"
#include "util/Log.h"

#include <future>
#include <memory>
#include <thread>

Scene::Scene(std::vector<ShaderProgram *> shaderPrograms)
    : m_shaderProgram(Controller::getShaderProgramByName("defaultProgram", shaderPrograms)),
      m_depthMapDirectionalFBO(SHADOW_RES)
{
    // Create 4 depthMaps
    for (int i = 0; i < 4; i++) {
        DepthMapCube *temp_depthMap = new DepthMapCube(SHADOW_RES);
        m_depthMapPointFBO.push_back(temp_depthMap);
    }

    // This will be removed in future when gloss maps are implemented
    m_shaderProgram->bind();
    m_shaderProgram->setUniform("u_material.shininess", 100.0f);
    m_shaderProgram->unbind();

    std::array modelDescriptors{
        ModelDescriptor{"fallback", "data/res/models/fallback.ffo"},
        ModelDescriptor{"backpack", "data/res/models/backpack.ffo"},
        ModelDescriptor{"ground", "data/res/models/wood_floor.ffo"},
        ModelDescriptor{"cube", "data/res/models/cube.ffo"},
        ModelDescriptor{"container", "data/res/models/container.ffo"},
    };

    {
        std::vector<std::future<void>> futures;
        std::mutex mutex;

        for (const auto &descriptor : modelDescriptors) {

            auto loadModel = [=, &mutex]() {
                ResourceId model = ResourceHandler::instance().registerResource<Model>(descriptor);

                Log::logger().info("Loaded model \"{}\": {}", descriptor.name, descriptor.path);

                std::lock_guard<std::mutex> lock(mutex);
                m_models.push_back(model);
            };

            futures.push_back(std::async(std::launch::async, loadModel));
        }
    }

    // TODO: use geometry shader instead of model and load skybox before models.
    Skybox::Prototype skyboxPrototype{"data/res/textures/skybox/"};
    m_skybox =
        new Skybox(skyboxPrototype, std::static_pointer_cast<Model>(ResourceHandler::instance().resource("cube")).get(),
                   Controller::getShaderProgramByName("skyboxProgram", shaderPrograms));

    Log::logger().info("Loaded skybox: {}", skyboxPrototype.texturePath);
    m_skybox->initializeOnGPU();

    std::array entityPrototypes{
        ModelEntity::Prototype{"backpack", "", {0., 1., 0.}, {}, 0.6, "backpack", "defaultProgram"},
        ModelEntity::Prototype{
            "container", "backpack", {10., 1., 0.}, {45., 45., 45.}, 1., "container", "defaultProgram"},
        ModelEntity::Prototype{"ground", "", {}, {}, 1., "ground", "defaultProgram"},
        ModelEntity::Prototype{"light", "", {}, {}, 1., "cube", "lightProgram"},
    };

    {
        for (auto &prototype : entityPrototypes) {
            // Get model
            const Model *currentModel =
                std::static_pointer_cast<Model>(ResourceHandler::instance().resource(prototype.modelName)).get();

            if (!currentModel) {
                // Apply fallback model (first model in vector)
                currentModel = std::static_pointer_cast<Model>(ResourceHandler::instance().resource("fallback"))
                                   .get(); // TODO rename fallbackModel

                Log::logger().warn("Model could not be found by name \"{}\"", prototype.modelName);
            }

            // Get shaderprogram
            ShaderProgram *currentProgram =
                Controller::getShaderProgramByName(prototype.shaderProgramName, shaderPrograms);

            if (!currentProgram) {
                currentProgram = Controller::getShaderProgramByName("basic", shaderPrograms);
            }

            ModelEntity *currentEntity = new ModelEntity(prototype, currentModel, currentProgram);

            Log::logger().info("Loaded entity \"{}\" with model \"{}\"", prototype.name, prototype.modelName);

            if (!prototype.parent.empty()) {
                Entity *parent = getEntityByName(prototype.parent);
                if (parent) {
                    parent->addChild(currentEntity);
                    currentEntity->setParent(parent);
                }
            } else
                m_entities.push_back(currentEntity);
        }
    }

    // Order important
    std::array<std::unique_ptr<Light::Prototype>, 2> lightPrototypes{
        std::unique_ptr<DirectionalLight::Prototype>(
            new DirectionalLight::Prototype{"directionalLight", {-0.2, -1.0, -0.3}, {1., 1., 1.}, 0.25}),
        std::unique_ptr<PointLight::Prototype>(
            new PointLight::Prototype{"pointLight0", "light", {0., 1., 0.}, {1., 1., 1.}, 7.5}),
    };

    std::vector<Light *> lights;
    {
        for (auto &prototype : lightPrototypes) {
            Light *currentLight;
            auto directionalPrototype = dynamic_cast<DirectionalLight::Prototype *>(prototype.get());
            if (directionalPrototype) {
                currentLight = new DirectionalLight(*directionalPrototype, m_shaderProgram);
            }
            auto pointPrototype = dynamic_cast<PointLight::Prototype *>(prototype.get());
            if (pointPrototype) {
                currentLight = new PointLight(*pointPrototype, m_shaderProgram);
                if (!pointPrototype->parent.empty()) {
                    Entity *parent = getEntityByName(pointPrototype->parent);
                    if (parent) {
                        parent->addChild(currentLight);
                        currentLight->setParent(parent);
                    } else {
                        Log::logger().warn("Could not find entity \"{}\"", pointPrototype->parent);
                    }
                }
            }
            lights.push_back(currentLight);
            Log::logger().info("Loaded light \"{}\"", prototype->name);
        }
    }
    m_lights = lights;
}

Scene::~Scene()
{
    // Iterate over depthMapPointFBO vector and delete all items
    for (auto it = m_depthMapPointFBO.begin(); it != m_depthMapPointFBO.end(); it++) {
        delete (*it);
    }
    // Iterate over models and entities and delete all items
    // for (auto it = m_models.begin(); it != m_models.end(); it++) {
    //     delete (*it);
    // }
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        delete (*it);
    }

    delete m_skybox;
}

void Scene::addEntity(ModelEntity *entity)
{
    m_entities.push_back(entity);
}

void Scene::removeEntityByName(const std::string &name)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            m_entities.erase(it);
            return;
        }
    }

    Log::logger().warn("Entity \"{}\" could not be removed", name);
}

void Scene::clearEntities()
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        m_entities.erase(it);
    }
}

void Scene::updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color, float intensity)
{
    std::vector<PointLight *> pointLights = getPointLights();
    pointLights[lightId]->setActive(active);
    pointLights[lightId]->setPosition(position);
    pointLights[lightId]->setIntensity(intensity);
    pointLights[lightId]->setColor(color);
}

void Scene::updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color)
{
    DirectionalLight *directionalLight = getDirectionalLight();
    directionalLight->setActive(active);
    directionalLight->setDirection(direction);
    directionalLight->setColor(color);
}

void Scene::draw(glm::mat4 viewProjMatrix, glm::vec3 viewPosition)
{
    // Draw all entities
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        (*it)->draw(viewProjMatrix, viewPosition);
    }

    // Calculate shadows (unclear if it should belong here or somewhere else)
    // calculateShadows();
}

void Scene::calculateShadows(ShaderProgram *directionalShaderProgram, ShaderProgram *pointShaderProgram)
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMapPointFBO[i]->getCubeMapTextureId());

        m_shaderProgram->unbind();
    }

    // Reset viewport size
    glViewport(VIEWPORT[0], VIEWPORT[1], VIEWPORT[2], VIEWPORT[3]);
    glCullFace(GL_FRONT);
}

ModelEntity *Scene::getEntityByName(const std::string &name)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    Log::logger().warn("Entity could not be found by unique name \"{}\"", name);
    return nullptr;
}

ModelEntity *Scene::getEntityById(uint32_t id)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getId() == id) {
            return *it;
        }
    }
    Log::logger().warn("Entity could not be found by ID \"{}\"", id);
    return nullptr;
}

std::vector<PointLight *> Scene::getPointLights()
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

DirectionalLight *Scene::getDirectionalLight()
{
    DirectionalLight *temp_directionalLight = nullptr;

    for (auto it = m_lights.begin(); it != m_lights.end(); it++) {
        temp_directionalLight = dynamic_cast<DirectionalLight *>(*it);
        if (temp_directionalLight)
            break;
    }

    return temp_directionalLight;
}

std::vector<ModelEntity *> Scene::getEntities()
{
    return m_entities;
}

Skybox *Scene::getSkybox()
{
    return m_skybox;
}
