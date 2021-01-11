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
        temp_models.push_back(current_model);
        std::cout << "Loaded Model \"" << model_name << "\" from \"" << model_path << "\"" << std::endl;
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
        if(!current_model)
            std::cout << "[Warning] Model could not be found by unique name \"" << entity_model << "\"" << std::endl;

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

Skybox *JsonParser::getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram)
{
    Skybox* temp_skybox;

    const Json::Value shaderProgramsJson = root["skybox"];

    std::string skybox_texturePath = shaderProgramsJson["texturePath"].asString();
    temp_skybox = new Skybox(cubeModel, skyboxProgram, skybox_texturePath.c_str());
    std::cout << "Loaded Skybox \"" << skybox_texturePath << "\"" << std::endl;

    return temp_skybox;
}
