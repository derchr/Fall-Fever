#include "Menu.h"
#include "JsonParser.h"
#include "eventActions.h"
#include "Helper.h"

#include <iostream>

Menu::Menu(Framebuffer *p_framebuffer, ShaderProgram *p_shaderProgram) :
    framebuffer(p_framebuffer), shaderProgram(p_shaderProgram)
{
    JsonParser screenParser("data/screens.json");
    screens = screenParser.getScreens(shaderProgram, framebuffer);
}

Menu::~Menu()
{
    // Iterate over screens delete all items
    for (auto it = screens.begin(); it != screens.end(); it++) {
        delete (*it);
    }
}

Screen *Menu::getScreenByName(const std::string& name) const
{
    if (m_screen_name_cache.find(name) != m_screen_name_cache.end())
        return m_screen_name_cache[name];

    for (auto it : screens) {
        if(it->getUniqueName() == name) {
            m_screen_name_cache[name] = it;
            return it;
        }
    }

    return nullptr;
}

Screen *Menu::getActiveScreen() const
{
    return activeScreen;
}

void Menu::showScreenByName(const std::string& name)
{
    Screen* screen = getScreenByName(name);

    if (!screen) return;

    screen->draw();
    activeScreen = screen;
}

void Menu::resetActiveScreen()
{
    activeScreen = nullptr;
}

void Menu::handleMouseButtonActionRegister(bool *mouseButtonActionRegister, Window* window)
{
    if (mouseButtonActionRegister[mouseButtonActions::leftClicked]) {
        auto widgets = activeScreen->getWidgets();
        for (auto it = widgets.begin(); it != widgets.end(); it++) {
            if ((*it)->isHovered(window)) {
                // std::cout << (*it)->getUniqueName() << " clicked!" << std::endl;
                if((*it)->getCallbackId() == 1)
                    resetActiveScreen();
                if((*it)->getCallbackId() == 2)
                    shouldExit = true;
            }
        }
    }
}

void Menu::writeWindowActions(bool *windowActionRegister)
{
    if (shouldExit)
        windowActionRegister[windowActions::windowShouldClose] = true;
}

void Menu::onPlayPressed()
{

}

void Menu::onExitPressed()
{

}
