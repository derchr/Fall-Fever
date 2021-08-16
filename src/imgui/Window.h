#pragma once

#include <string>

namespace Imgui {

class Window
{
public:
    Window(const std::string &title);
    void render();

protected:
    virtual void addWidgets() = 0;

    std::string m_title;
};

} // namespace Imgui
