#pragma once

class GLFWwindow;

class Window
{
public:
    Window();
    ~Window();

    GLFWwindow *getGLFWwindow();

    int getWindowWidth();
    int getWindowHeight();
    float getWindowAspectRatio();

    bool isWindowResized();
    void updateWindowDimensions();

private:
    static void glfw_error_callback(int error, const char *description);
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void setCatchedCursor(bool value);

    GLFWwindow *m_window;

    int m_posX, m_posY;
    int m_width, m_height;

    bool m_mouseCatched = true;
    bool m_wireFrameMode = false;
};
