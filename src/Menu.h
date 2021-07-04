#pragma once

#include <string>
#include <unordered_map>

#include "Framebuffer.h"
#include "JsonParser.h"
#include "Screen.h"
#include "EventHandler.h"

class Menu
{
public:
    Menu(Framebuffer *p_framebuffer, ShaderProgram *p_shaderProgram);
    ~Menu();

    Screen *getScreenByName(const std::string &name) const;
    void showScreenByName(const std::string &name);

    Screen *getActiveScreen() const;
    void writeWindowActions(WindowActionMap &windowActionMap);

    void resetActiveScreen();

    void handleMouseButtonActionMap(const MouseButtonActionMap &mouseButtonActionMap, Window *window);

    void onPlayPressed();
    void onExitPressed();

private:
    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;

    bool shouldExit = false;

    std::vector<Screen *> screens;
    Screen *activeScreen;
    /*Screen *loadingScreen;
    Screen *mainMenuScreen;
    Screen *optionMenuScreen;
    Screen *pauseMenuScreen;*/

    mutable std::unordered_map<std::string, Screen *> m_screen_name_cache;
};
