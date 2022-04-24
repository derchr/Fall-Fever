#pragma once

#include "Widget.h"
#include "resources/Texture.h"

#include <string>
#include <unordered_map>
#include <vector>

class Menu;
class FrameBuffer;
class ShaderProgram;

class Screen
{
public:
    struct Prototype
    {
        std::string name;
        std::vector<Widget::Prototype> widgetPrototypes;
    };

    Screen(Prototype prototype, FrameBuffer *framebuffer, ShaderProgram *shaderProgram);
    ~Screen();

    void addWidget(Widget *widget);
    void draw() const;

    const std::string &getUniqueName() const;
    std::vector<Widget *> getWidgets() const;
    Widget *getWidgetByName(const std::string &name) const;

private:
    uint32_t m_id;
    static uint32_t s_idCounter;
    std::string m_uniqueName;

    FrameBuffer *m_frameBuffer;
    ShaderProgram *m_shaderProgram;

    std::vector<ResourceId> m_textures;
    std::vector<Widget *> m_widgets;

    mutable std::unordered_map<std::string, Widget *> m_widgetNameCache;
};
