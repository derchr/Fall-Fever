#include <iostream>
#include <glad/glad.h>

#include "Window.h"

Window::Window() {
    width = 800; height = 600;

    window = glfwCreateWindow(width, height, "Fall-Fever", NULL, NULL);
    if(!window) {
        std::cout << "Failed to create window" << std::endl;
    }

    // Create OpenGL context
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}

Window::~Window() {
    glfwDestroyWindow(window);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}  
