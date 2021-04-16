#include "Screen.h"
#include "helper.h"

uint32_t Screen::id_counter = 0;

Screen::Screen(const std::string &name, std::vector<Widget*> widgets, Framebuffer *framebuffer, ShaderProgram *shaderProgram) :
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

const std::string& Screen::getUniqueName() const
{
    return unique_name;
}

std::vector<Widget*> Screen::getWidgets() const
{
    return widgets;
}

Widget *Screen::getWidgetByName(const std::string& name) const
{
    if (m_widget_name_cache.find(name) != m_widget_name_cache.end())
        return m_widget_name_cache[name];

    for (auto it : widgets) {
        if(it->getUniqueName() == name) {
            m_widget_name_cache[name] = it;
            return it;
        }
    }
    return nullptr;
}

void Screen::addWidget(Widget *widget)
{
    widgets.push_back(widget);
}

void Screen::draw() const
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

