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

    Screen *getScreenByName(const char *unique_name);
    void showScreenByName(const char *unique_name);

    void resetActiveScreen();

    void handleMouseButtonActionRegister(bool *mouseButtonActionRegister, Window* window);

private:
    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;

    std::vector<Screen*> screens;
    Screen *activeScreen;
    /*Screen *loadingScreen;
    Screen *mainMenuScreen;
    Screen *optionMenuScreen;
    Screen *pauseMenuScreen;*/
};
