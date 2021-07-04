#pragma once

#include "FrameBuffer.h"
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
    double m_posX, m_posY, m_width, m_height;

    std::string m_uniqueName;

    uint16_t m_callbackId;

    std::vector<Vertex> m_widgetVertices;
    std::vector<uint32_t> m_widgetIndices;
    std::vector<Texture *> m_widgetTextures;

    Mesh *m_widgetMesh;
};
