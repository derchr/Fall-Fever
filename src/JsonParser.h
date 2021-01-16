#pragma once

#include "Model.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "Light.h"

#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class JsonParser
{
public:
    JsonParser(std::string path);
    ~JsonParser();

    std::vector<Model*> getModels();
    std::vector<Entity*> getEntities(std::vector<Model*> &models, std::vector<ShaderProgram*> shaderPrograms);
    std::vector<Light*> getLights(ShaderProgram* shaderProgram);
    Skybox *getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram);

    std::vector<ShaderProgram*> getShaderPrograms();

private:
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
};
