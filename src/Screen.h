#pragma once

#include <unordered_map>

#include "ShaderProgram.h"
#include "Framebuffer.h"
#include "Widget.h"

class Menu;

class Screen
{
public:
    Screen(const std::string& name, std::vector<Widget*> widgets, Framebuffer* framebuffer, ShaderProgram* shaderProgram);
    ~Screen();
    
    void addWidget(Widget *widget);
    void draw() const;

    const std::string& getUniqueName() const;
    std::vector<Widget*> getWidgets() const;
    Widget *getWidgetByName(const std::string& name) const;
    
private:
    uint32_t id;
    static uint32_t id_counter;
    std::string unique_name;

    Framebuffer *framebuffer;
    ShaderProgram *shaderProgram;
    
    std::vector<Texture*> textures;
    std::vector<Widget*> widgets;

    mutable std::unordered_map<std::string, Widget*> m_widget_name_cache;
};
