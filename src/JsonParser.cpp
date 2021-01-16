#include "JsonParser.h"

#include <fstream>
#include <iostream>

JsonParser::JsonParser(std::string path)
{
    std::ifstream file(path.c_str(), std::ifstream::binary);

    if (!file) {
        std::cout << "Error reading file \"" << path << "\"." << std::endl;
        return;
    }

    std::string errs;
    rbuilder["collectComments"] = false;
    bool parsingSuccessful = Json::parseFromStream(rbuilder, file, &root, &errs);
    file.close();
    if (!parsingSuccessful) {
        std::cout << "Failed to parse file\n" << errs << std::endl;
        return;
    }
}

JsonParser::~JsonParser()
{

}

std::vector<Model*> JsonParser::getModels()
{
    std::vector<Model*> temp_models;

    const Json::Value modelsJson = root["models"];

    for (unsigned int index = 0; index < modelsJson.size(); index++) {
        std::string model_name = modelsJson[index]["unique_name"].asString();
        std::string model_path = modelsJson[index]["path"].asString();
        Model *current_model = new Model(model_name, model_path);
        if(current_model) {
            temp_models.push_back(current_model);
            std::cout << "Loaded Model \"" << model_name << "\" from \"" << model_path << "\"" << std::endl;
        }
    }

    return temp_models;
}

std::vector<Entity*> JsonParser::getEntities(std::vector<Model*> &models, std::vector<ShaderProgram*> shaderPrograms)
{
    std::vector<Entity*> temp_entities;

    const Json::Value entitiesJson = root["entities"];

    for (unsigned int index = 0; index < entitiesJson.size(); index++) {
        std::string entity_name = entitiesJson[index]["unique_name"].asString();
        std::string entity_model = entitiesJson[index]["model"].asString();
        std::string entity_shaderProgram = entitiesJson[index]["shaderProgram"].asString();
        glm::vec3 entitiy_position = {}, entity_rotation = {};
        float entity_scale = 1.0f;

        ShaderProgram *shaderProgram = nullptr;
        for (auto it = shaderPrograms.begin(); it != shaderPrograms.end(); it++) {
            if((*it)->getUniqueName() == entity_shaderProgram) {
                shaderProgram = *it;
            }
        }
        if(!shaderProgram)
            std::cout << "[Warning] ShaderProgram could not be found by name \"" << entity_shaderProgram << "\"" << std::endl;

        Model *current_model = nullptr;
        for (auto it = models.begin(); it != models.end(); it++) {
            if((*it)->getUniqueName() == entity_model) {
                current_model = *it;
            }
        }
        if(!current_model) {
            // Apply fallback model (first model in vector)
            current_model = models[0];
            std::cout << "[Warning] Model could not be found by unique name \"" << entity_model << "\"" << std::endl;
        }

        const Json::Value positionJson = entitiesJson[index]["position"];
        const Json::Value rotationJson = entitiesJson[index]["rotation"];
        const Json::Value scaleJson = entitiesJson[index]["scale"];
        if(!positionJson.empty()) {
            entitiy_position.x = positionJson[0].asFloat();
            entitiy_position.y = positionJson[1].asFloat();
            entitiy_position.z = positionJson[2].asFloat();
        }
        if(!rotationJson.empty()) {
            entity_rotation.s = rotationJson[0].asFloat();
            entity_rotation.t = rotationJson[1].asFloat();
            entity_rotation.p = rotationJson[2].asFloat();
        }
        if(!scaleJson.empty()) {
            entity_scale = scaleJson.asFloat();
        }

        Entity *current_entity = new Entity(entity_name, current_model, shaderProgram);
        current_entity->setPosition(entitiy_position);
        current_entity->setRotation(entity_rotation);
        current_entity->setScale(entity_scale);
        temp_entities.push_back(current_entity);
        std::cout << "Loaded Entity \"" << entity_name << "\" with model \"" << entity_model << "\"" << std::endl;
    }

    return temp_entities;
}

std::vector<ShaderProgram *> JsonParser::getShaderPrograms()
{
    std::vector<ShaderProgram*> temp_shaderPrograms;

    const Json::Value shaderProgramsJson = root["shaderPrograms"];

    for (unsigned int index = 0; index < shaderProgramsJson.size(); index++) {
        std::string shaderProgram_name = shaderProgramsJson[index]["unique_name"].asString();
        std::string shaderProgram_vertexPath = shaderProgramsJson[index]["vertexPath"].asString();
        std::string shaderProgram_fragmentPath = shaderProgramsJson[index]["fragmentPath"].asString();
        std::string shaderProgram_geometryPath = shaderProgramsJson[index]["geometryPath"].asString();

        ShaderProgram *current_shaderProgram;
        if(shaderProgram_geometryPath.empty()) {
            current_shaderProgram = new ShaderProgram(shaderProgram_name, shaderProgram_vertexPath, shaderProgram_fragmentPath);
        } else {
            current_shaderProgram = new ShaderProgram(shaderProgram_name, shaderProgram_vertexPath, shaderProgram_geometryPath, shaderProgram_fragmentPath);
        }
        temp_shaderPrograms.push_back(current_shaderProgram);
        std::cout << "Loaded ShaderProgram \"" << shaderProgram_name << "\"" << std::endl;
    }

    return temp_shaderPrograms;
}

std::vector<Light*> JsonParser::getLights(ShaderProgram* shaderProgram)
{
    std::vector<Light*> temp_lights;
    glm::vec3 light_direction = {1.0f, 0.0f, 0.0f};
    glm::vec3 light_position = {};
    glm::vec3 light_color = {1.0f, 1.0f, 1.0f};
    float light_intensity = 10.0f;

    const Json::Value directionalLightsJson = root["directionalLight"];

    const Json::Value directionJson = directionalLightsJson["direction"];
    Json::Value colorJson = directionalLightsJson["color"];
    Json::Value intensityJson = directionalLightsJson["intensity"];

    if(!intensityJson.empty()) {
        light_intensity = intensityJson.asFloat();
    }
    if(!directionJson.empty()) {
        light_direction.x = directionJson[0].asFloat();
        light_direction.y = directionJson[1].asFloat();
        light_direction.z = directionJson[2].asFloat();
    }
    if(!colorJson.empty()) {
        light_color.x = colorJson[0].asFloat();
        light_color.y = colorJson[1].asFloat();
        light_color.z = colorJson[2].asFloat();
    }

    DirectionalLight *current_directionalLight = new DirectionalLight(light_direction, light_color, light_intensity, shaderProgram);
    current_directionalLight->setActive(true);
    temp_lights.push_back(current_directionalLight);

    // Pointlights
    const Json::Value pointLightsJson = root["pointLights"];


    unsigned int index = 0;
    for (; index < pointLightsJson.size(); index++) {
        PointLight *current_pointLight;

        const Json::Value positionJson = pointLightsJson[index]["position"];
        colorJson = pointLightsJson[index]["color"];
        intensityJson = pointLightsJson[index]["intensity"];

        if(!intensityJson.empty()) {
            light_intensity = intensityJson.asFloat();
        }
        if(!positionJson.empty()) {
            light_position.x = positionJson[0].asFloat();
            light_position.y = positionJson[1].asFloat();
            light_position.z = positionJson[2].asFloat();
        }
        if(!colorJson.empty()) {
            light_color.x = colorJson[0].asFloat();
            light_color.y = colorJson[1].asFloat();
            light_color.z = colorJson[2].asFloat();
        }

        current_pointLight = new PointLight(light_position, light_color, light_intensity, shaderProgram);
        current_pointLight->setActive(true);
        temp_lights.push_back(current_pointLight);
    }

    // In case there aren't enough PointLights defined in the Json file
    for(; NUM_POINT_LIGHTS - index > 0; index++) {
        const glm::vec3 default_position(0.0f);
        const glm::vec3 default_color(1.0f);
        const float default_intensity = 10.0f;
        PointLight *current_pointLight = new PointLight(default_position, default_color, default_intensity, shaderProgram);
        current_pointLight->setActive(false);
        temp_lights.push_back(current_pointLight);
    }

    return temp_lights;
}

std::vector<Screen*> JsonParser::getScreens(ShaderProgram *menuProgram, Framebuffer *framebuffer)
{
    std::vector<Screen*> temp_screens;

    const Json::Value loadingScreenJson = root["loadingScreen"];
    const Json::Value mainMenuScreenJson = root["mainMenuScreen"];

    std::string name;
    Screen *screen;

    name = "loadingScreen";
    screen = new Screen(name, getWidgetsFromScreen(loadingScreenJson), framebuffer, menuProgram);
    temp_screens.push_back(screen);

    name = "mainMenuScreen";
    screen = new Screen(name, getWidgetsFromScreen(mainMenuScreenJson), framebuffer, menuProgram);
    temp_screens.push_back(screen);

    return temp_screens;
}

std::vector<Widget*> JsonParser::getWidgetsFromScreen(const Json::Value &screenJson)
{
    std::vector<Widget*> temp_widgets;

    // Iterate over widgets
    unsigned int index = 0;
    for (; index < screenJson.size(); index++) {
        const Json::Value currentWidgetJson = screenJson[index];
        const Json::Value currentWidgetTextureJson = currentWidgetJson["texture"];
        const Json::Value currentWidgetPosition = currentWidgetJson["position"];
        const Json::Value currentWidgetDimensions = currentWidgetJson["dimensions"];
        std::string name = currentWidgetJson["unique_name"].asString();
        Texture *currentWidgetTexture = new Texture(currentWidgetTextureJson.asString().c_str(), textureType::texture_diffuse);
        Widget *currentWidget = new Widget(
            name,
            currentWidgetTexture,
            currentWidgetPosition[0].asFloat(),
            currentWidgetPosition[1].asFloat(),
            currentWidgetDimensions[0].asFloat(),
            currentWidgetDimensions[1].asFloat()
        );
        temp_widgets.push_back(currentWidget);
    }
    return temp_widgets;
}

Skybox *JsonParser::getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram)
{
    Skybox* temp_skybox;

    const Json::Value shaderProgramsJson = root["skybox"];

    std::string skybox_texturePath = shaderProgramsJson["texturePath"].asString();
    temp_skybox = new Skybox(cubeModel, skyboxProgram, skybox_texturePath.c_str());
    std::cout << "Loaded Skybox \"" << skybox_texturePath << "\"" << std::endl;

    return temp_skybox;
}
