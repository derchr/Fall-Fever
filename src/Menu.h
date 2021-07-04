#pragma once

#include <string>
#include <unordered_map>

#include "FrameBuffer.h"
#include "JsonParser.h"
#include "Screen.h"
#include "EventHandler.h"

class Menu
{
public:
    Menu(FrameBuffer *p_framebuffer, ShaderProgram *p_shaderProgram);
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
    FrameBuffer *m_frameBuffer;
    ShaderProgram *m_shaderProgram;

    bool m_shouldExit = false;

    std::vector<Screen *> m_screens;
    Screen *m_activeScreen;
    /*Screen *loadingScreen;
    Screen *mainMenuScreen;
    Screen *optionMenuScreen;
    Screen *pauseMenuScreen;*/

    mutable std::unordered_map<std::string, Screen *> m_screenNameCache;
};
