#pragma once

#include <GLFW/glfw3.h>

class Window {

public:

    Window();
    ~Window();

    GLFWwindow * getGLFWwindow() { return window; }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

private:

    GLFWwindow *window;

    int width, height;

};