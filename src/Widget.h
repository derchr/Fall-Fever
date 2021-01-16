#pragma once

#include "Texture.h"
#include "Mesh.h"
#include "Framebuffer.h"
#include "Window.h"

class Widget
{
public:
    Widget(std::string &name, Texture *texture, float x, float y, float w, float h);
    ~Widget();
    
    void draw(ShaderProgram *shaderProgram);

    std::string getUniqueName();

    bool isHovered(Window *window);

private:
    double x, y, w, h;

    std::string unique_name;
    
    std::vector<Vertex> widgetVertices;
    std::vector<uint32_t> widgetIndices;
    std::vector<Texture*> widgetTextures;
    
    Mesh *widgetMesh;
};
