#pragma once

#include "EventHandler.h"

#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    ~Window();

    GLFWwindow *getGLFWwindow();

    int getWindowWidth();
    int getWindowHeight();
    float getWindowAspectRatio();
    bool getMouseIsCatched();

    bool isWindowResized();
    void updateWindowDimensions();

    void handleWindowActionMap(WindowActionMap &windowActionMap);

private:
    static void glfw_error_callback(int error, const char *description);
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void setCatchedCursor(bool value);

    GLFWwindow *window;

    int m_posX, m_posY;
    int m_width, m_height;

    bool m_mouseCatched = true;
    bool m_wireFrameMode = false;
};
