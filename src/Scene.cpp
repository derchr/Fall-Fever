#include "Scene.h"
#include "Controller.h"
#include "Entity.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "resources/Model.h"
#include "resources/ResourceHandler.h"
#include "util/Log.h"

#include <future>
#include <memory>
#include <thread>

Scene::Scene(std::vector<std::shared_ptr<ShaderProgram>> shaderPrograms)
    : m_shaderProgram(*Controller::getShaderProgramByName("defaultProgram", shaderPrograms))
{
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

            auto loadModel = [=, this, &mutex]() {
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
    m_skybox = std::make_shared<Skybox>(
        skyboxPrototype, std::static_pointer_cast<Model>(ResourceHandler::instance().resource("cube")).get(),
        Controller::getShaderProgramByName("skyboxProgram", shaderPrograms).get());

    Log::logger().info("Loaded skybox: {}", skyboxPrototype.texturePath);
    m_skybox->initializeOnGPU();

    std::array entityPrototypes{
        ModelEntity::Prototype{"backpack", {0., 1., 0.}, {}, 0.6F, "backpack", "defaultProgram"},
        ModelEntity::Prototype{"container", {10., 1., 0.}, {45., 45., 45.}, 1., "container", "defaultProgram"},
        ModelEntity::Prototype{"ground", {}, {}, 1., "ground", "defaultProgram"},
        ModelEntity::Prototype{"light", {}, {}, 1., "cube", "lightProgram"},
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
            auto currentProgram = Controller::getShaderProgramByName(prototype.shaderProgramName, shaderPrograms);

            if (!currentProgram) {
                currentProgram = Controller::getShaderProgramByName("basic", shaderPrograms);
            }

            m_entities.push_back(std::make_shared<ModelEntity>(prototype, currentModel, currentProgram.get()));

            Log::logger().info("Loaded entity \"{}\" with model \"{}\"", prototype.name, prototype.modelName);
        }
    }

    // Order important
    std::array<std::unique_ptr<Light::Prototype>, 2> lightPrototypes{
        std::unique_ptr<DirectionalLight::Prototype>(
            new DirectionalLight::Prototype{"directionalLight", {-0.2, -1.0, -0.3}, {1., 1., 1.}, 0.25}),
        std::unique_ptr<PointLight::Prototype>(
            new PointLight::Prototype{"pointLight0", "light", {0., 1., 0.}, {1., 1., 1.}, 7.5}),
    };

    std::vector<std::shared_ptr<Light>> lights;
    {
        for (auto &prototype : lightPrototypes) {
            std::shared_ptr<Light> currentLight;
            auto directionalPrototype = dynamic_cast<DirectionalLight::Prototype *>(prototype.get());
            if (directionalPrototype) {
                currentLight = std::make_shared<DirectionalLight>(*directionalPrototype, &m_shaderProgram);
            }
            auto pointPrototype = dynamic_cast<PointLight::Prototype *>(prototype.get());
            if (pointPrototype) {
                currentLight = std::make_shared<PointLight>(*pointPrototype, &m_shaderProgram);
            }

            lights.push_back(currentLight);
            Log::logger().info("Loaded light \"{}\"", prototype->name);
        }
    }
    m_lights = lights;
}

void Scene::updatePointLight(unsigned int lightId, bool active, glm::vec3 position, glm::vec3 color, float intensity)
{
    auto pointLights = getPointLights();
    pointLights[lightId]->setActive(active);
    pointLights[lightId]->setPosition(position);
    pointLights[lightId]->setIntensity(intensity);
    pointLights[lightId]->setColor(color);
}

void Scene::updateDirectionalLight(bool active, glm::vec3 direction, glm::vec3 color)
{
    auto directionalLight = getDirectionalLight();
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
}

std::shared_ptr<ModelEntity> Scene::getEntityByName(const std::string &name)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getUniqueName() == name) {
            return *it;
        }
    }
    Log::logger().warn("Entity could not be found by unique name \"{}\"", name);
    return nullptr;
}

std::shared_ptr<ModelEntity> Scene::getEntityById(uint32_t id)
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        if ((*it)->getId() == id) {
            return *it;
        }
    }
    Log::logger().warn("Entity could not be found by ID \"{}\"", id);
    return nullptr;
}

std::vector<std::shared_ptr<PointLight>> Scene::getPointLights()
{
    std::vector<std::shared_ptr<PointLight>> temp_pointLights;

    for (auto it = m_lights.begin(); it != m_lights.end(); it++) {
        auto temp_pointLight = std::dynamic_pointer_cast<PointLight>(*it);
        if (temp_pointLight) {
            temp_pointLights.push_back(temp_pointLight);
        }
    }

    return temp_pointLights;
}

std::shared_ptr<DirectionalLight> Scene::getDirectionalLight()
{
    std::shared_ptr<DirectionalLight> temp_directionalLight;

    for (auto it = m_lights.begin(); it != m_lights.end(); it++) {
        temp_directionalLight = std::dynamic_pointer_cast<DirectionalLight>(*it);
        if (temp_directionalLight)
            break;
    }

    return temp_directionalLight;
}

std::vector<std::shared_ptr<ModelEntity>> Scene::getEntities()
{
    return m_entities;
}

std::shared_ptr<Skybox> Scene::getSkybox()
{
    return m_skybox;
}
