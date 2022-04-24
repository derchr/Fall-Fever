#pragma once

#include <jsoncpp/json/json.h>
#include <memory>
#include <string>
#include <vector>

#include "Entity.h"
#include "Light.h"
#include "Screen.h"
#include "ShaderProgram.h"
#include "Widget.h"
#include "resources/Model.h"

class Screen;
class ShaderProgram;
class FrameBuffer;

class JsonParser
{
public:
    JsonParser(const std::string &jsonFilepath);
    ~JsonParser();

    std::vector<ModelDescriptor> getModelDescriptors() const;
    std::vector<ModelEntity::Prototype> getEntityPrototypes() const;
    std::vector<std::unique_ptr<Light::Prototype>> getLightPrototypes() const;
    std::vector<Screen::Prototype> getScreenPrototypes() const;
    Skybox::Prototype getSkyboxPrototype() const;

    std::vector<ShaderProgram::Prototype> getShaderProgramPrototypes();

private:
    std::vector<Widget::Prototype> getWidgetPrototypesFromScreen(const Json::Value &screenJson) const;

    Json::Value m_root;
    Json::CharReaderBuilder m_rbuilder;
};
