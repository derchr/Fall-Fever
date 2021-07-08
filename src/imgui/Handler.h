#pragma once

#include <memory>
#include <vector>

class GLFWwindow;

namespace Imgui {

class Window;

class Handler
{
public:
    Handler(GLFWwindow *window);
    ~Handler();

    void addImguiWindow(std::shared_ptr<Window> window);
    void renderWindows();

private:
    std::vector<std::shared_ptr<Window>> m_windows;
    GLFWwindow *m_GLFWwindow;
};

} // namespace Imgui