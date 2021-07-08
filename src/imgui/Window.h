#pragma once

namespace Imgui {

class Window
{
public:
    Window();
    void render();

protected:
    virtual void addWidgets() = 0;
};

} // namespace Imgui
