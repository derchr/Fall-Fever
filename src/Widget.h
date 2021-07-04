#pragma once

#include "Framebuffer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Window.h"
#include "definitions/eventActions.h"

class Menu;

class Widget
{
public:
    Widget(std::string &name, Texture *texture, float x, float y, float w, float h, uint16_t callbackId);
    ~Widget();

    void draw(ShaderProgram *shaderProgram);

    std::string getUniqueName();
    uint16_t getCallbackId();

    bool isHovered(Window *window);

private:
    double x, y, w, h;

    std::string unique_name;

    uint16_t callbackId;

    std::vector<Vertex> widgetVertices;
    std::vector<uint32_t> widgetIndices;
    std::vector<Texture *> widgetTextures;

    Mesh *widgetMesh;
};
