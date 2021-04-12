#pragma once

#include <string>

#include "Screen.h"
#include "Framebuffer.h"
#include "JsonParser.h"
#include "eventActions.h"

class Menu
{
public:
    Menu(Framebuffer *p_framebuffer, ShaderProgram *p_shaderProgram);
    ~Menu();

    Screen *getScreenByName(const std::string& unique_name);
    void showScreenByName(const std::string& unique_name);

    Screen *getActiveScreen();
    void writeWindowActions(bool *windowActionRegister);

    void resetActiveScreen();

    void handleMouseButtonActionRegister(bool *mouseButtonActionRegister, Window* window);

    void onPlayPressed();
    void onExitPressed();

private:
    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;

    bool shouldExit = false;

    std::vector<Screen*> screens;
    Screen *activeScreen;
    /*Screen *loadingScreen;
    Screen *mainMenuScreen;
    Screen *optionMenuScreen;
    Screen *pauseMenuScreen;*/
};
