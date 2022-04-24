#pragma once

#include "definitions/models.h"
#include "resources/Texture.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Menu;
class ShaderProgram;
class Window;
class Mesh;

class Widget
{
public:
    struct Prototype
    {
        std::string name;
        glm::vec2 position;
        glm::vec2 dimensions;
        TextureDescriptor texturePrototype;
        uint16_t callBackId; // TODO: will be removed...
    };

    Widget(Prototype prototype, ResourceId texture);

    void draw(ShaderProgram *shaderProgram);

    std::string getUniqueName();
    uint16_t getCallbackId();

    bool isHovered(Window *window);

private:
    glm::vec2 m_position;
    glm::vec2 m_dimensions;

    std::string m_uniqueName;

    uint16_t m_callbackId;

    std::vector<ResourceId> m_widgetTextures;
};
