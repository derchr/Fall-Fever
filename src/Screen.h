#pragma once

#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "Widget.h"

class Menu;

class Screen
{
public:
    Screen(std::string &name, std::vector<Widget*> widgets, Framebuffer *framebuffer, ShaderProgram *shaderProgram);
    ~Screen();
    
    void addWidget(Widget *widget);
    void draw();

    std::string getUniqueName();
    std::vector<Widget*> getWidgets();
    Widget *getWidgetByName(const char* name);
    
private:
    uint32_t id;
    static uint32_t id_counter;
    std::string unique_name;

    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;
    
    std::vector<Texture*> textures;
    std::vector<Widget*> widgets;
};
