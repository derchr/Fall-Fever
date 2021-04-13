#pragma once

#include "Model.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "Light.h"
#include "Screen.h"

#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class JsonParser
{
public:
    JsonParser(const std::string& path);
    ~JsonParser();

    std::vector<Model*> getModels();
    std::vector<Entity*> getEntities(std::vector<Model*> &models, std::vector<ShaderProgram*> shaderPrograms);
    std::vector<Light*> getLights(ShaderProgram *shaderProgram);
    std::vector<Screen*> getScreens(ShaderProgram *menuProgram, Framebuffer *framebuffer);
    Skybox *getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram);

    std::vector<ShaderProgram*> getShaderPrograms();

private:
    std::vector<Widget*> getWidgetsFromScreen(const Json::Value &screenJson);

private:
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
};
