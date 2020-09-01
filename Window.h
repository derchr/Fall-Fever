#pragma once

#include <GLFW/glfw3.h>

class Window {

public:

    Window();
    ~Window();

    GLFWwindow * getGLFWwindow() { return window; }

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

    GLFWwindow *window;

    int width, height;

};