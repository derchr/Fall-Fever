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

    // Enable z buffer
    glEnable(GL_DEPTH_TEST);

    #ifdef _DEBUG
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, NULL);
	#endif

    glViewport(0, 0, width, height);

    // Tell GLFW which function to call when window is resized
    // Currently not used...
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}

Window::~Window() {
    glfwDestroyWindow(window);
}

// This function is called when the window gets resized
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}  

void Window::openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	(void)length; (void)userParam;
    
    if(severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
    std::cout << "[OpenGL Error]" << std::endl
        << "Message: " << message << std::endl
        << "Source: " << source << std::endl
        << "Type: " << type << std::endl
        << "ID: " << id << std::endl
        << "Severity: " << severity << std::endl
        << std::endl;
}
