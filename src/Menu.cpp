#include "Menu.h"
#include "JsonParser.h"

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

void Menu::showScreenByName(const char *unique_name)
{
    for (auto it = screens.begin(); it != screens.end(); it++) {
        if((*it)->getUniqueName() == unique_name) {
            (*it)->draw();
            break;
        }
    }
}
