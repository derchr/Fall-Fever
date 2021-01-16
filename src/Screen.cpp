#include "Screen.h"

uint32_t Screen::id_counter = 0;

Screen::Screen(std::string &name, std::vector<Widget*> widgets, Framebuffer *framebuffer, ShaderProgram *shaderProgram) :
    unique_name(name),
    framebuffer(framebuffer),
    shaderProgram(shaderProgram),
    widgets(widgets)
{
    id = id_counter++;
}

Screen::~Screen()
{
    // Iterate over Widgets and Textures to delete all of them
    for(auto it = widgets.begin(); it != widgets.end(); it++) {
        delete *it;
    }
    for(auto it = textures.begin(); it != textures.end(); it++) {
        delete *it;
    }
}

std::string Screen::getUniqueName()
{
    return unique_name;
}

void Screen::addWidget(Widget *widget)
{
    widgets.push_back(widget);
}

void Screen::draw()
{
    framebuffer->setExposureCorrection(false);
    framebuffer->bind();
    
    for(auto it = widgets.begin(); it != widgets.end(); it++) {
        (*it)->draw(shaderProgram);
    }

    framebuffer->unbind();
    framebuffer->render();
    framebuffer->setExposureCorrection(true);
}

