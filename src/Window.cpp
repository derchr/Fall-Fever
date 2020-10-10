#include <iostream>
#include <glad/glad.h>

#include "Window.h"
#include "eventActions.h"
#include "defines.h"


Window::Window() {
    width = INIT_WINDOW_WIDTH; height = INIT_WINDOW_HEIGHT;

    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if(!window) {
        std::cout << "Failed to create window" << std::endl;
    }

    glfwGetWindowPos(window, &posX, &posY);

    // Create OpenGL context
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // Enable z buffer
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    // Enable multisampling (a bit redundant because most graphics drivers do this automatically)
    glEnable(GL_MULTISAMPLE);

    // Disable mouse cursor
    #ifdef _DEBUG
    mouseCatched = false;
    #endif
    setCatchedCursor(mouseCatched);

    // Enable primitive gamma correction
    // glEnable(GL_FRAMEBUFFER_SRGB);

    // Maximize in release build
    #ifndef _DEBUG
    glfwMaximizeWindow(window);
    #endif

    #ifdef _DEBUG
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, NULL);
	#endif

    glViewport(0, 0, width, height);

    // Tell GLFW which function to call when window is resized
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}

Window::~Window() {
    glfwDestroyWindow(window);
}

bool Window::checkWindowWasResized() {
    int new_width, new_height, new_posx, new_posy;
    glfwGetFramebufferSize(window, &new_width, &new_height);
    glfwGetWindowPos(window, &new_posx, &new_posy);
    if(new_width == width && new_height == height && new_posx == posX && new_posy == posY)
        return 0;

    width = new_width; height = new_height; posX = new_posx; posY = new_posy;
    glViewport(0, 0, width, height);
    
    return 1;
}

void Window::setCatchedCursor(bool value) {
    if(value) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::handleActionRegister(bool *windowActionRegister) {
    if(windowActionRegister[wireFrameToggle]) {
        windowActionRegister[wireFrameToggle] = 0;
        wireFrameMode = !wireFrameMode;
        if(wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if(windowActionRegister[mouseCatchToggle]) {
        windowActionRegister[mouseCatchToggle] = 0;
        mouseCatched = !mouseCatched;
        setCatchedCursor(mouseCatched);
    }

}

// This function is called when the window gets resized (currently not used)
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
