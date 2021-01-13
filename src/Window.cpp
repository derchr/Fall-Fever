#include <iostream>
#include <glad/glad.h>

#include "Window.h"
#include "eventActions.h"
#include "defines.h"
#include "helper.h"


Window::Window()
{
    // Initialize GLFW
    if (!glfwInit()) {
        exit(-1);
    }

    width = INIT_WINDOW_WIDTH;
    height = INIT_WINDOW_HEIGHT;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    // Maximize in release build
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#endif

    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
    }

    // Wait for window to maximize (in case)
    Helper::sleep(1000);

    glfwGetWindowPos(window, &posX, &posY);
    glfwGetWindowSize(window, &width, &height);

    // Create OpenGL context
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

#ifdef _DEBUG
    // Disable mouse cursor
    mouseCatched = false;

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openGLDebugCallback, NULL);
#endif

    // Enable z buffer
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    // Enable multisampling (a bit redundant because most graphics drivers do this automatically)
    glEnable(GL_MULTISAMPLE);

    // Disable VSync since my sleep function handles this
    glfwSwapInterval(0);

    setCatchedCursor(mouseCatched);

    glViewport(0, 0, width, height);

    // Tell GLFW which function to call when window is resized
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::isWindowResized()
{
    int new_width, new_height, new_posx, new_posy;
    glfwGetFramebufferSize(window, &new_width, &new_height);
    glfwGetWindowPos(window, &new_posx, &new_posy);

    return !(new_width == width && new_height == height && new_posx == posX && new_posy == posY);
}

void Window::updateWindowDimensions()
{
    glfwGetFramebufferSize(window, &width, &height);
    glfwGetWindowPos(window, &posX, &posY);

    glViewport(0, 0, width, height);
}


void Window::setCatchedCursor(bool value)
{
    if (value) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::handleActionRegister(bool *windowActionRegister)
{
    if (windowActionRegister[wireFrameToggle]) {
        windowActionRegister[wireFrameToggle] = 0;
        wireFrameMode = !wireFrameMode;
        if (wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (windowActionRegister[mouseCatchToggle]) {
        windowActionRegister[mouseCatchToggle] = 0;
        mouseCatched = !mouseCatched;
        setCatchedCursor(mouseCatched);
    }
}

// GLFW error function
void Window::error_callback(int error, const char *description)
{
    fprintf(stderr, "[Error] GLFW Error %d: %s\n", error, description);
}

// This function is called when the window gets resized (currently not used)
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

void Window::openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    (void)length;
    (void)userParam;

    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
        std::cout << "[OpenGL Error]" << std::endl
                  << "Message: " << message << std::endl
                  << "Source: " << source << std::endl
                  << "Type: " << type << std::endl
                  << "ID: " << id << std::endl
                  << "Severity: " << severity << std::endl
                  << std::endl;
}

GLFWwindow* Window::getGLFWwindow()
{
    return window;
}

int Window::getWindowWidth()
{
    return width;
}

int Window::getWindowHeight()
{
    return height;
}

float Window::getWindowAspectRatio()
{
    return (float) width / (float) height;
}

bool Window::getMouseIsCatched()
{
    return mouseCatched;
}
