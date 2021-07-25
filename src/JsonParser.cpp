#include "JsonParser.h"

#include <fstream>
#include <iostream>

JsonParser::JsonParser(const std::string &jsonFilepath)
{
    std::ifstream file(jsonFilepath.c_str(), std::ifstream::binary);

    if (!file) {
        std::cout << "Error reading file \"" << jsonFilepath << "\"." << std::endl;
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

std::vector<ModelEntity::Prototype> JsonParser::getEntityPrototypes() const
{
    std::vector<ModelEntity::Prototype> entityPrototypes;

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

        ModelEntity::Prototype prototype(entityName, entitiyPosition, entityRotation, entityScale, entityModel,
                                         entityShaderProgram);

        entityPrototypes.push_back(prototype);
    }

    return entityPrototypes;
}

std::vector<ShaderProgram::Prototype> JsonParser::getShaderProgramPrototypes()
{
    std::vector<ShaderProgram::Prototype> prototypes;

    const Json::Value shaderProgramsJson = m_root["shaderPrograms"];

    for (unsigned int index = 0; index < shaderProgramsJson.size(); index++) {
        std::string shaderProgram_name = shaderProgramsJson[index]["unique_name"].asString();
        std::string shaderProgram_vertexPath = shaderProgramsJson[index]["vertexPath"].asString();
        std::string shaderProgram_fragmentPath = shaderProgramsJson[index]["fragmentPath"].asString();
        std::string shaderProgram_geometryPath = shaderProgramsJson[index]["geometryPath"].asString();

        ShaderProgram::Prototype prototype{shaderProgram_name, shaderProgram_vertexPath, shaderProgram_fragmentPath,
                                           shaderProgram_geometryPath};

        prototypes.push_back(prototype);
    }

    return prototypes;
}

std::vector<std::unique_ptr<Light::Prototype>> JsonParser::getLightPrototypes() const
{
    std::vector<std::unique_ptr<Light::Prototype>> prototypes;
    glm::vec3 direction = {1.0f, 0.0f, 0.0f};
    glm::vec3 position = {};
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    float intensity = 10.0f;

    const Json::Value directionalLightsJson = m_root["directionalLight"];

    const Json::Value directionJson = directionalLightsJson["direction"];
    Json::Value colorJson = directionalLightsJson["color"];
    Json::Value intensityJson = directionalLightsJson["intensity"];

    if (!intensityJson.empty()) {
        intensity = intensityJson.asFloat();
    }
    if (!directionJson.empty()) {
        direction.x = directionJson[0].asFloat();
        direction.y = directionJson[1].asFloat();
        direction.z = directionJson[2].asFloat();
    }
    if (!colorJson.empty()) {
        color.x = colorJson[0].asFloat();
        color.y = colorJson[1].asFloat();
        color.z = colorJson[2].asFloat();
    }

    auto prototype = std::unique_ptr<Light::Prototype>(new DirectionalLight::Prototype{direction, color, intensity});

    prototypes.push_back(std::move(prototype));

    // Pointlights
    const Json::Value pointLightsJson = m_root["pointLights"];

    int index = 0;
    for (; index < (int)pointLightsJson.size(); index++) {
        const Json::Value positionJson = pointLightsJson[index]["position"];
        colorJson = pointLightsJson[index]["color"];
        intensityJson = pointLightsJson[index]["intensity"];

        if (!intensityJson.empty()) {
            intensity = intensityJson.asFloat();
        }
        if (!positionJson.empty()) {
            position.x = positionJson[0].asFloat();
            position.y = positionJson[1].asFloat();
            position.z = positionJson[2].asFloat();
        }
        if (!colorJson.empty()) {
            color.x = colorJson[0].asFloat();
            color.y = colorJson[1].asFloat();
            color.z = colorJson[2].asFloat();
        }

        auto prototype = std::unique_ptr<Light::Prototype>(new PointLight::Prototype{position, color, intensity});

        prototypes.push_back(std::move(prototype));
    }

    // In case there aren't enough PointLights defined in the Json file
    for (; NUM_POINT_LIGHTS - index > 0; index++) {
        const glm::vec3 default_position(0.0f);
        const glm::vec3 default_color(1.0f);
        const float default_intensity = 10.0f;
        auto prototype = std::unique_ptr<PointLight::Prototype>(
            new PointLight::Prototype{default_position, default_color, default_intensity});

        prototypes.push_back(std::move(prototype));
    }

    return prototypes;
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

Skybox::Prototype JsonParser::getSkyboxPrototype() const
{
    const Json::Value shaderProgramsJson = m_root["skybox"];

    std::string texturePath = shaderProgramsJson["texturePath"].asString();
    Skybox::Prototype prototype{texturePath};

    return prototype;
}
