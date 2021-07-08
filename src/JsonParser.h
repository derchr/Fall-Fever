#pragma once

#include "Model.h"

#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class Entity;
class Light;
class Screen;
class Skybox;
class ShaderProgram;
class FrameBuffer;
class Widget;

class JsonParser
{
public:
    JsonParser(const std::string &path);
    ~JsonParser();

    std::vector<Model::Prototype> getModelPrototypes() const;
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
