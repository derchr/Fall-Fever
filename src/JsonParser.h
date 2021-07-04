#pragma once

#include "Entity.h"
#include "Light.h"
#include "Model.h"
#include "Screen.h"
#include "ShaderProgram.h"

#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class JsonParser
{
public:
    JsonParser(const std::string &path);
    ~JsonParser();

    std::vector<Model *> getModels();
    std::vector<Entity *> getEntities(std::vector<Model *> &models, std::vector<ShaderProgram *> shaderPrograms);
    std::vector<Light *> getLights(ShaderProgram *shaderProgram);
    std::vector<Screen *> getScreens(ShaderProgram *menuProgram, FrameBuffer *framebuffer);
    Skybox *getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram);

    std::vector<ShaderProgram *> getShaderPrograms();

private:
    std::vector<Widget *> getWidgetsFromScreen(const Json::Value &screenJson);

    Json::Value m_root;
    Json::CharReaderBuilder m_rbuilder;
};
