#include "Screen.h"

Screen::Screen(Framebuffer *framebuffer, ShaderProgram *shaderProgram) :
    framebuffer(framebuffer), shaderProgram(shaderProgram)
{
    
}

Screen::~Screen()
{
    
}

void Screen::addWidget(Widget* widget)
{
    widgets.push_back(widget);
}

void Screen::draw()
{
    framebuffer->setExposureCorrection(false);
    framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for(auto it = widgets.begin(); it != widgets.end(); it++) {
        (*it)->draw(shaderProgram);
    }

    framebuffer->unbind();
    framebuffer->render();
    framebuffer->setExposureCorrection(true);
}

