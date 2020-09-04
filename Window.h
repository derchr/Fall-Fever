#pragma once

#include <GLFW/glfw3.h>

class Window {

public:

    Window();
    ~Window();

    GLFWwindow * getGLFWwindow() { return window; }

    int getWindowWidth() { return width; }
    int getWindowHeight() { return height; }

    // Beware: side effect!
    bool checkWindowWasResized();

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

    void setGrabbedCursor(bool value);

    GLFWwindow *window;

    int posX, posY;
    int width, height;

};