#include "Screen.h"
#include "FrameBuffer.h"
#include "Helper.h"
#include "Menu.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Widget.h"

uint32_t Screen::s_idCounter = 0;

Screen::Screen(Prototype prototype, FrameBuffer *framebuffer, ShaderProgram *shaderProgram)
    : m_id(s_idCounter++), m_uniqueName(prototype.name), m_frameBuffer(framebuffer), m_shaderProgram(shaderProgram)
{
    for (auto &prototype : prototype.widgetPrototypes) {
        auto texturePrototype = prototype.texturePrototype;
        Texture *currentTexture = new Texture({texturePrototype.texturePath, texturePrototype.textureType});
        currentTexture->initializeOnGPU();

        Widget *currentWidget = new Widget(prototype, currentTexture);
        m_widgets.push_back(currentWidget);
    }
}

Screen::~Screen()
{
    // Iterate over Widgets and Textures to delete all of them
    for (auto it = m_widgets.begin(); it != m_widgets.end(); it++) {
        delete *it;
    }
    for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
        delete *it;
    }
}

const std::string &Screen::getUniqueName() const
{
    return m_uniqueName;
}

std::vector<Widget *> Screen::getWidgets() const
{
    return m_widgets;
}

Widget *Screen::getWidgetByName(const std::string &name) const
{
    if (m_widgetNameCache.find(name) != m_widgetNameCache.end())
        return m_widgetNameCache[name];

    for (auto it : m_widgets) {
        if (it->getUniqueName() == name) {
            m_widgetNameCache[name] = it;
            return it;
        }
    }
    return nullptr;
}

void Screen::addWidget(Widget *widget)
{
    m_widgets.push_back(widget);
}

void Screen::draw() const
{
    m_frameBuffer->setExposureCorrection(false);
    m_frameBuffer->bind();

    for (auto it = m_widgets.begin(); it != m_widgets.end(); it++) {
        (*it)->draw(m_shaderProgram);
    }

    m_frameBuffer->unbind();
    m_frameBuffer->drawOnEntireScreen();
    m_frameBuffer->setExposureCorrection(true);
}
