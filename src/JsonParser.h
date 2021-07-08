#pragma once

#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

#include "Entity.h"
#include "Model.h"
#include "Screen.h"
#include "Widget.h"

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
    std::vector<Entity::Prototype> getEntityPrototypes() const;
    std::vector<Light *> getLights(ShaderProgram *shaderProgram); // should be under entities too
    std::vector<Screen::Prototype> getScreenPrototypes() const;
    Skybox *getSkybox(Model *cubeModel, ShaderProgram *skyboxProgram);

    std::vector<ShaderProgram *> getShaderPrograms();

private:
    std::vector<Widget::Prototype> getWidgetPrototypesFromScreen(const Json::Value &screenJson) const;

    Json::Value m_root;
    Json::CharReaderBuilder m_rbuilder;
};
