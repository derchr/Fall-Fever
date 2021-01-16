#pragma once

#include <string>

#include "Screen.h"
#include "Framebuffer.h"
#include "JsonParser.h"

class Menu
{
public:
    Menu(Framebuffer *p_framebuffer, ShaderProgram *p_shaderProgram);
    ~Menu();

    void showScreenByName(const char *unique_name);

private:
    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;

    std::vector<Screen*> screens;
    /*Screen *loadingScreen;
    Screen *mainMenuScreen;
    Screen *optionMenuScreen;
    Screen *pauseMenuScreen;*/
};
