#include "JsonParser.h"
#include "Entity.h"
#include "Light.h"
#include "Model.h"
#include "Screen.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Widget.h"

#include <fstream>
#include <iostream>

JsonParser::JsonParser(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ifstream::binary);

    if (!file) {
        std::cout << "Error reading file \"" << path << "\"." << std::endl;
        return;
    }

    std::string errs;
    m_rbuilder["collectComments"] = false;
    bool parsingSuccessful = Json::parseFromStream(m_rbuilder, file, &m_root, &errs);
    file.close();
    if (!parsingSuccessful) {
        std::cout << "Failed to parse file\n" << errs << std::endl;
        return;
    }
}

JsonParser::~JsonParser()
{}

std::vector<Model::Prototype> JsonParser::getModelPrototypes() const
{
    const Json::Value modelsJson = m_root["models"];

    std::vector<Model::Prototype> modelPrototypes;

    for (unsigned int index = 0; index < modelsJson.size(); index++) {
        std::string modelName = modelsJson[index]["unique_name"].asString();
        std::string modelPath = modelsJson[index]["path"].asString();
        Model::Prototype prototype{modelName, modelPath};
        modelPrototypes.push_back(prototype);
    }

    return modelPrototypes;
}

std::vector<Entity::Prototype> JsonParser::getEntityPrototypes() const
{
    std::vector<Entity::Prototype> entityPrototypes;

    const Json::Value entitiesJson = m_root["entities"];

    for (unsigned int index = 0; index < entitiesJson.size(); index++) {
        std::string entityName = entitiesJson[index]["unique_name"].asString();
        std::string entityModel = entitiesJson[index]["model"].asString();
        std::string entityShaderProgram = entitiesJson[index]["shaderProgram"].asString();
        glm::vec3 entitiyPosition = {}, entityRotation = {};
        float entityScale = 1.0f;

        const Json::Value positionJson = entitiesJson[index]["position"];
        const Json::Value rotationJson = entitiesJson[index]["rotation"];
        const Json::Value scaleJson = entitiesJson[index]["scale"];
        if (!positionJson.empty()) {
            entitiyPosition.x = positionJson[0].asFloat();
            entitiyPosition.y = positionJson[1].asFloat();
            entitiyPosition.z = positionJson[2].asFloat();
        }
        if (!rotationJson.empty()) {
            entityRotation.s = rotationJson[0].asFloat();
            entityRotation.t = rotationJson[1].asFloat();
            entityRotation.p = rotationJson[2].asFloat();
        }
        if (!scaleJson.empty()) {
            entityScale = scaleJson.asFloat();
        }

        Entity::Prototype prototype{entityName,      entityModel,    entityShaderProgram,
                                    entitiyPosition, entityRotation, entityScale};

        entityPrototypes.push_back(prototype);
    }

    return entityPrototypes;
}

std::vector<ShaderProgram *> JsonParser::getShaderPrograms()
{
    std::vector<ShaderProgram *> temp_shaderPrograms;

    const Json::Value shaderProgramsJson = m_root["shaderPrograms"];

    for (unsigned int index = 0; index < shaderProgramsJson.size(); index++) {
        std::string shaderProgram_name = shaderProgramsJson[index]["unique_name"].asString();
        std::string shaderProgram_vertexPath = shaderProgramsJson[index]["vertexPath"].asString();
        std::string shaderProgram_fragmentPath = shaderProgramsJson[index]["fragmentPath"].asString();
        std::string shaderProgram_geometryPath = shaderProgramsJson[index]["geometryPath"].asString();

        ShaderProgram *current_shaderProgram;
        if (shaderProgram_geometryPath.empty()) {
            current_shaderProgram =
                new ShaderProgram(shaderProgram_name, shaderProgram_vertexPath, shaderProgram_fragmentPath);
        } else {
            current_shaderProgram = new ShaderProgram(shaderProgram_name, shaderProgram_vertexPath,
                                                      shaderProgram_geometryPath, shaderProgram_fragmentPath);
        }
        temp_shaderPrograms.push_back(current_shaderProgram);
        std::cout << "Loaded ShaderProgram \"" << shaderProgram_name << "\"" << std::endl;
    }

    return temp_shaderPrograms;
}

std::vector<Light *> JsonParser::getLights(ShaderProgram *shaderProgram)
{
    std::vector<Light *> temp_lights;
    glm::vec3 light_direction = {1.0f, 0.0f, 0.0f};
    glm::vec3 light_position = {};
    glm::vec3 light_color = {1.0f, 1.0f, 1.0f};
    float light_intensity = 10.0f;

    const Json::Value directionalLightsJson = m_root["directionalLight"];

    const Json::Value directionJson = directionalLightsJson["direction"];
    Json::Value colorJson = directionalLightsJson["color"];
    Json::Value intensityJson = directionalLightsJson["intensity"];

    if (!intensityJson.empty()) {
        light_intensity = intensityJson.asFloat();
    }
    if (!directionJson.empty()) {
        light_direction.x = directionJson[0].asFloat();
        light_direction.y = directionJson[1].asFloat();
        light_direction.z = directionJson[2].asFloat();
    }
    if (!colorJson.empty()) {
        light_color.x = colorJson[0].asFloat();
        light_color.y = colorJson[1].asFloat();
        light_color.z = colorJson[2].asFloat();
    }

    DirectionalLight *current_directionalLight =
        new DirectionalLight(light_direction, light_color, light_intensity, shaderProgram);
    current_directionalLight->setActive(true);
    temp_lights.push_back(current_directionalLight);

    // Pointlights
    const Json::Value pointLightsJson = m_root["pointLights"];

    int index = 0;
    for (; index < (int)pointLightsJson.size(); index++) {
        PointLight *current_pointLight;

        const Json::Value positionJson = pointLightsJson[index]["position"];
        colorJson = pointLightsJson[index]["color"];
        intensityJson = pointLightsJson[index]["intensity"];

        if (!intensityJson.empty()) {
            light_intensity = intensityJson.asFloat();
        }
        if (!positionJson.empty()) {
            light_position.x = positionJson[0].asFloat();
            light_position.y = positionJson[1].asFloat();
            light_position.z = positionJson[2].asFloat();
        }
        if (!colorJson.empty()) {
            light_color.x = colorJson[0].asFloat();
            light_color.y = colorJson[1].asFloat();
            light_color.z = colorJson[2].asFloat();
        }

        current_pointLight = new PointLight(light_position, light_color, light_intensity, shaderProgram);
        current_pointLight->setActive(true);
        temp_lights.push_back(current_pointLight);
    }

    // In case there aren't enough PointLights defined in the Json file
    for (; NUM_POINT_LIGHTS - index > 0; index++) {
        const glm::vec3 default_position(0.0f);
        const glm::vec3 default_color(1.0f);
        const float default_intensity = 10.0f;
        PointLight *current_pointLight =
            new PointLight(default_position, default_color, default_intensity, shaderProgram);
        current_pointLight->setActive(false);
        temp_lights.push_back(current_pointLight);
    }

    return temp_lights;
}

std::vector<Screen::Prototype> JsonParser::getScreenPrototypes() const
{
    std::vector<Screen::Prototype> screenPrototypes;

    const Json::Value loadingScreenJson = m_root["loadingScreen"];
    const Json::Value mainMenuScreenJson = m_root["mainMenuScreen"];

    std::string name;

    name = "loadingScreen";
    Screen::Prototype loadingScreen = Screen::Prototype{name, getWidgetPrototypesFromScreen(loadingScreenJson)};
    screenPrototypes.push_back(loadingScreen);

    name = "mainMenuScreen";
    Screen::Prototype mainMenuScreen = Screen::Prototype{name, getWidgetPrototypesFromScreen(mainMenuScreenJson)};
    screenPrototypes.push_back(mainMenuScreen);

    return screenPrototypes;
}

std::vector<Widget::Prototype> JsonParser::getWidgetPrototypesFromScreen(const Json::Value &screenJson) const
{
    std::vector<Widget::Prototype> widgetPrototypes;

    // Iterate over widgets
    for (unsigned int index = 0; index < screenJson.size(); index++) {
        const Json::Value currentWidgetJson = screenJson[index];
        const Json::Value currentWidgetTextureJson = currentWidgetJson["texture"];
        const Json::Value currentWidgetPosition = currentWidgetJson["position"];
        const Json::Value currentWidgetDimensions = currentWidgetJson["dimensions"];
        std::string name = currentWidgetJson["unique_name"].asString();
        glm::vec2 position(currentWidgetPosition[0].asFloat(), currentWidgetPosition[1].asFloat());
        glm::vec2 dimensions(currentWidgetDimensions[0].asFloat(), currentWidgetDimensions[1].asFloat());
        uint16_t callBackId = currentWidgetJson["callbackId"].asUInt();
        Texture::Prototype texturePrototype{currentWidgetTextureJson.asString(), TextureType::Diffuse};

        Widget::Prototype widgetPrototype{name, position, dimensions, texturePrototype, callBackId};

        widgetPrototypes.push_back(widgetPrototype);
    }
    return widgetPrototypes;
}

Skybox *JsonParser::getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram)
{
    Skybox *temp_skybox;

    const Json::Value shaderProgramsJson = m_root["skybox"];

    std::string skybox_texturePath = shaderProgramsJson["texturePath"].asString();
    temp_skybox = new Skybox(cubeModel, skyboxProgram, skybox_texturePath.c_str());
    std::cout << "Loaded Skybox \"" << skybox_texturePath << "\"" << std::endl;

    return temp_skybox;
}
