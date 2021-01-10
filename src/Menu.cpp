#include "Menu.h"

Menu::Menu(Framebuffer *p_framebuffer, ShaderProgram *p_shaderProgram) :
    framebuffer(p_framebuffer), shaderProgram(p_shaderProgram)
{
    mainMenuScreen = new Screen(screenType::mainMenu, framebuffer, shaderProgram);
    pauseMenuScreen = new Screen(screenType::pauseMenu, framebuffer, shaderProgram);
    loadingScreen = new Screen(screenType::loadingScreen, framebuffer, shaderProgram);
    optionMenuScreen = new Screen(screenType::optionMenu, framebuffer, shaderProgram);
}

Menu::~Menu()
{
    delete mainMenuScreen;
    delete pauseMenuScreen;
    delete loadingScreen;
    delete optionMenuScreen;
}

void Menu::showLoadingScreen()
{
    loadingScreen->draw();
}
