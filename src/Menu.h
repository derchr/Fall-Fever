#pragma once

#include "Screen.h"
#include "Framebuffer.h"

class Menu
{
public:
    Menu(Framebuffer *p_framebuffer, ShaderProgram *p_shaderProgram);
    ~Menu();

    void showLoadingScreen();

private:
    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;

    Screen *loadingScreen;
    Screen *mainMenuScreen;
    Screen *optionMenuScreen;
    Screen *pauseMenuScreen;
};
