#include "Menu.h"
#include "FrameBuffer.h"
#include "Helper.h"
#include "JsonParser.h"
#include "Screen.h"
#include "ShaderProgram.h"
#include "Widget.h"
#include "Window.h"
#include "definitions/eventActions.h"
#include "util/Log.h"

Menu::Menu(FrameBuffer *p_framebuffer, ShaderProgram *p_shaderProgram)
    : m_frameBuffer(p_framebuffer), m_shaderProgram(p_shaderProgram)
{
    JsonParser screenParser("data/screens.json");
    std::vector<Screen::Prototype> screenPrototypes = screenParser.getScreenPrototypes();

    for (auto &prototype : screenPrototypes) {
        auto currentScreen = new Screen(prototype, p_framebuffer, p_shaderProgram);
        m_screens.push_back(currentScreen);
    }
}

Menu::~Menu()
{
    // Iterate over screens delete all items
    for (auto it = m_screens.begin(); it != m_screens.end(); it++) {
        delete (*it);
    }
}

Screen *Menu::getScreenByName(const std::string &name) const
{
    if (m_screenNameCache.find(name) != m_screenNameCache.end())
        return m_screenNameCache[name];

    for (auto it : m_screens) {
        if (it->getUniqueName() == name) {
            m_screenNameCache[name] = it;
            return it;
        }
    }

    return nullptr;
}

Screen *Menu::getActiveScreen() const
{
    return m_activeScreen;
}

void Menu::showScreenByName(const std::string &name)
{
    Screen *screen = getScreenByName(name);

    if (!screen)
        return;

    screen->draw();
    m_activeScreen = screen;
}

void Menu::resetActiveScreen()
{
    m_activeScreen = nullptr;
}

void Menu::handleMouseButtonActionMap(const MouseButtonActionMap &mouseButtonActionMap, Window *window)
{
    if (mouseButtonActionMap.at(MouseButtonAction::LeftClicked)) {
        auto widgets = m_activeScreen->getWidgets();
        for (auto it = widgets.begin(); it != widgets.end(); it++) {
            if ((*it)->isHovered(window)) {
                if ((*it)->getCallbackId() == 1)
                    resetActiveScreen();
                if ((*it)->getCallbackId() == 2)
                    m_shouldExit = true;
            }
        }
    }
}

void Menu::writeWindowActions(WindowActionMap &windowActionMap)
{
    if (m_shouldExit)
        windowActionMap[WindowAction::WindowShouldClose] = true;
}

void Menu::onPlayPressed()
{}

void Menu::onExitPressed()
{}
