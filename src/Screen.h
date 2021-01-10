#pragma once

#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "Widget.h"

enum screenType
{
    loadingScreen,
    mainMenu,
    optionMenu,
    pauseMenu,
    SCREEN_TYPE_NUM_ITEMS
};

class Screen
{
public:
    Screen(screenType p_type, Framebuffer *framebuffer, ShaderProgram *shaderProgram);
    ~Screen();
    
    void addWidget(Widget *widget);
    void draw();
    
private:
    screenType type;

    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;
    
    std::vector<Texture*> textures;
    std::vector<Widget*> widgets;
};
