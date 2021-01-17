#include "Menu.h"
#include "JsonParser.h"
#include "eventActions.h"
#include "helper.h"
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

Screen *Menu::getScreenByName(const char* unique_name)
{
    for (auto it = screens.begin(); it != screens.end(); it++) {
        if((*it)->getUniqueName() == unique_name) {
            return *it;
        }
    }
    return nullptr;
}

Screen *Menu::getActiveScreen()
{
    return activeScreen;
}

void Menu::showScreenByName(const char *unique_name)
{
    auto it = screens.begin();
    for (; it != screens.end(); it++) {
        if((*it)->getUniqueName() == unique_name) {
            (*it)->draw();
            break;
        }
    }
    activeScreen = *it;
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
                std::cout << (*it)->getUniqueName() << " clicked!" << std::endl;
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
    std::cout << "Hello, from Widget play in Menu :)" << std::endl;
}

void Menu::onExitPressed()
{

}
