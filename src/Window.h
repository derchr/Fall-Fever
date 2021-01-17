#pragma once

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

    void handleWindowActionRegister(bool *windowActionRegister);

private:
    static void glfw_error_callback(int error, const char *description);
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void setCatchedCursor(bool value);

private:
    GLFWwindow *window;

    int posX, posY;
    int width, height;

    bool mouseCatched = true;
    bool wireFrameMode = false;
};
