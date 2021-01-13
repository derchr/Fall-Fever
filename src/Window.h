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

    void handleActionRegister(bool *windowActionRegister);

private:
    static void error_callback(int error, const char *description);
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
    void setCatchedCursor(bool value);

private:
    GLFWwindow *window;

    int posX, posY;
    int width, height;

    bool mouseCatched = true;
    bool wireFrameMode = false;
};
