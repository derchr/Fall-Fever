#pragma once

#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "Widget.h"

class Screen
{
public:
    Screen(Framebuffer *framebuffer, ShaderProgram *shaderProgram);
    ~Screen();
    
    void addWidget(Widget *widget);
    void draw();
    
private:
    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;
    
    std::vector<Widget*> widgets;
};
