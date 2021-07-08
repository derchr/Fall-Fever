#pragma once

#include "Texture.h"
#include "definitions/models.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Menu;
class ShaderProgram;
class Texture;
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
        Texture::Prototype texturePrototype;
        uint16_t callBackId; // TODO: will be removed...
    };

    Widget(Prototype prototype, Texture *texture);
    ~Widget();

    void draw(ShaderProgram *shaderProgram);

    std::string getUniqueName();
    uint16_t getCallbackId();

    bool isHovered(Window *window);

private:
    glm::vec2 m_position;
    glm::vec2 m_dimensions;

    std::string m_uniqueName;

    uint16_t m_callbackId;

    std::vector<Vertex> m_widgetVertices;
    std::vector<uint32_t> m_widgetIndices;
    std::vector<Texture *> m_widgetTextures;

    Mesh *m_widgetMesh;
};
