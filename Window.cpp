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

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    glViewport(0, 0, width, height);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Tell GLFW which function to call when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}

Window::~Window() {
    glfwDestroyWindow(window);
}

// This function is called when the window gets resized
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}  
