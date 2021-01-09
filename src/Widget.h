#pragma once

#include "Texture.h"
#include "Mesh.h"
#include "Framebuffer.h"

class Widget
{
public:
    Widget(Texture *texture, float x, float y, float w, float h);
    ~Widget();
    
    void draw(ShaderProgram *shaderProgram);

private:
    double x, y, w, h;
    
    std::vector<Vertex> widgetVertices;
    std::vector<uint32_t> widgetIndices;
    std::vector<Texture*> widgetTextures;
    
    Mesh *widgetMesh;
};
