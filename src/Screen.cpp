#include "Screen.h"

Screen::Screen(screenType p_type, Framebuffer *framebuffer, ShaderProgram *shaderProgram) :
    type(p_type), framebuffer(framebuffer), shaderProgram(shaderProgram)
{
    switch(type) {
        case loadingScreen: {
            Texture *temp_loadingScreenTex = new Texture("res/textures/loading.png", textureType::texture_diffuse);
            textures.push_back(temp_loadingScreenTex);
            Widget *temp_loadingScreenWidget = new Widget(temp_loadingScreenTex, 0.f, 0.f, 1.f, 1.f);
            widgets.push_back(temp_loadingScreenWidget);
            break;
        }

        default:
            break;
    }
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

void Screen::addWidget(Widget *widget)
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

