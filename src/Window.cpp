#include "Window.h"
#include "Helper.h"
#include "ShaderProgram.h"
#include "definitions/window.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>

Window::Window()
{
    // Initialize GLFW
    if (!glfwInit())
        exit(-1);

    m_width = INIT_WINDOW_WIDTH;
    m_height = INIT_WINDOW_HEIGHT;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    // Maximize in release build
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#endif

    m_window = glfwCreateWindow(m_width, m_height, "OpenGL", NULL, NULL);
    if (!m_window)
        Log::logger().critical("Failed to create window");

    // Wait for window to maximize (in case)
    // Helper::sleep(1000);

    glfwGetWindowPos(m_window, &m_posX, &m_posY);
    glfwGetWindowSize(m_window, &m_width, &m_height);

    // Create OpenGL context
    glfwMakeContextCurrent(m_window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Log::logger().critical("Failed to initialize GLAD");
        exit(-1);
    }

#ifdef _DEBUG
    Log::logger().debug("OpenGL version: {}", glGetString(GL_VERSION));
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(Helper::gl_debug_callback, NULL);

    // Disable mouse cursor
    m_mouseCatched = false;
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

    setCatchedCursor(m_mouseCatched);

    glViewport(0, 0, m_width, m_height);

    // Tell GLFW which function to call when window is resized
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::isWindowResized()
{
    int new_width, new_height, new_posx, new_posy;
    glfwGetFramebufferSize(m_window, &new_width, &new_height);
    glfwGetWindowPos(m_window, &new_posx, &new_posy);

    return !(new_width == m_width && new_height == m_height && new_posx == m_posX && new_posy == m_posY);
}

void Window::updateWindowDimensions()
{
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    glfwGetWindowPos(m_window, &m_posX, &m_posY);

    glViewport(0, 0, m_width, m_height);
}

void Window::setCatchedCursor(bool value)
{
    if (value) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::handleWindowActionMap(WindowActionMap &windowActionMap)
{
    if (windowActionMap.at(WindowAction::WireFrameToggle)) {
        windowActionMap[WindowAction::WireFrameToggle] = false;
        m_wireFrameMode = !m_wireFrameMode;
        if (m_wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (windowActionMap.at(WindowAction::MouseCatchToggle)) {
        windowActionMap[WindowAction::MouseCatchToggle] = false;
        m_mouseCatched = !m_mouseCatched;
        setCatchedCursor(m_mouseCatched);
    }

    if (windowActionMap.at(WindowAction::WindowShouldClose)) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

// GLFW error function
void Window::glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "[Error] GLFW Error %d: %s\n", error, description);
}

// This function is called when the window gets resized (currently not used)
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

GLFWwindow *Window::getGLFWwindow()
{
    return m_window;
}

int Window::getWindowWidth()
{
    return m_width;
}

int Window::getWindowHeight()
{
    return m_height;
}

float Window::getWindowAspectRatio()
{
    return (float)m_width / (float)m_height;
}

bool Window::getMouseIsCatched()
{
    return m_mouseCatched;
}
