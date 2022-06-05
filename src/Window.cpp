#include "Window.h"
#include "Helper.h"
#include "ShaderProgram.h"
#include "definitions/window.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>

Window::Window()
{
    // Initialize GLFW
    if (glfwInit() == 0) {
        std::terminate();
    }

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

    m_glfw_window = std::shared_ptr<GLFWwindow>(glfwCreateWindow(m_width, m_height, "OpenGL", nullptr, nullptr),
                                                [](GLFWwindow *window) { glfwDestroyWindow(window); });
    if (!m_glfw_window) {
        Log::logger().critical("Failed to create window");
    }

    // Wait for window to maximize (in case)
    // Helper::sleep(1000);

    glfwGetWindowPos(m_glfw_window.get(), &m_posX, &m_posY);
    glfwGetWindowSize(m_glfw_window.get(), &m_width, &m_height);

    // Create OpenGL context
    glfwMakeContextCurrent(m_glfw_window.get());

    // Initialize GLAD
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        Log::logger().critical("Failed to initialize GLAD");
        std::terminate();
    }

#ifdef _DEBUG
    Log::logger().debug("OpenGL version: {}", glGetString(GL_VERSION));
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(Helper::gl_debug_callback, nullptr);

    // Disable mouse cursor
    m_mouse_catched = false;
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

    set_catched_cursor(m_mouse_catched);

    glViewport(0, 0, m_width, m_height);

    glfwSetWindowUserPointer(m_glfw_window.get(), this);
    glfwSetKeyCallback(m_glfw_window.get(), key_callback);
    glfwSetMouseButtonCallback(m_glfw_window.get(), mouse_button_callback);
    glfwSetCursorPosCallback(m_glfw_window.get(), mouse_cursor_callback);

    // Tell GLFW which function to call when window is resized
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

auto Window::dimensions_changed() const -> bool
{
    int new_width{};
    int new_height{};
    int new_posx{};
    int new_posy{};

    glfwGetFramebufferSize(m_glfw_window.get(), &new_width, &new_height);
    glfwGetWindowPos(m_glfw_window.get(), &new_posx, &new_posy);

    return !(new_width == m_width && new_height == m_height && new_posx == m_posX && new_posy == m_posY);
}

void Window::update_dimensions()
{
    glfwGetFramebufferSize(m_glfw_window.get(), &m_width, &m_height);
    glfwGetWindowPos(m_glfw_window.get(), &m_posX, &m_posY);

    glViewport(0, 0, m_width, m_height);
}

void Window::set_catched_cursor(bool value)
{
    if (value) {
        glfwSetInputMode(m_glfw_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_glfw_window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    m_mouse_catched = value;
}

// GLFW error function
void Window::glfw_error_callback(int error, const char *description)
{
    Log::logger().warn("GLFW [%d]: %s\n", error, description);
}

// This function is called when the window gets resized (currently not used)
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

void Window::key_callback(GLFWwindow *glfw_window, int key, int scancode, int action,
                          int mods) // NOLINT(bugprone-easily-swappable-parameters)
{
    (void)mods;
    (void)scancode;

    auto &window = *static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
    } else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        window.set_catched_cursor(!window.m_mouse_catched);
    } else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        window.m_wire_frame_mode = !window.m_wire_frame_mode;
        glPolygonMode(GL_FRONT_AND_BACK, window.m_wire_frame_mode ? GL_LINE : GL_FILL);
    } else if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        window.m_key_input[key] = (action == GLFW_PRESS);
    }
}

void Window::mouse_button_callback(GLFWwindow *glfw_window, int button, int action,
                                   int mods) // NOLINT(bugprone-easily-swappable-parameters)
{
    (void)mods;
    (void)button;
    (void)action;

    auto &window = *static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    (void)window;
}

void Window::mouse_cursor_callback(GLFWwindow *glfw_window, double xpos, double ypos)
{
    auto &window = *static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));

    double deltaCursorPosX = xpos - window.m_last_cursor_pos_x;
    double deltaCursorPosY = -(ypos - window.m_last_cursor_pos_y);

    window.m_last_cursor_pos_x = xpos;
    window.m_last_cursor_pos_y = ypos;

    // Check if this is the first VALID mouse event after window being resized
    if (window.m_first_mouse_input && !(deltaCursorPosX == 0 && deltaCursorPosY == 0)) {
        window.m_first_mouse_input = false;
        deltaCursorPosX = 0.0;
        deltaCursorPosY = 0.0;
    }

    deltaCursorPosX *= MOUSE_SENSITIVITY;
    deltaCursorPosY *= MOUSE_SENSITIVITY;

    auto &[deltaX, deltaY] = window.m_mouse_cursor_input;
    deltaX += deltaCursorPosX;
    deltaY += deltaCursorPosY;
}

auto Window::dimensions() const -> std::pair<unsigned, unsigned>
{
    return {m_width, m_height};
}

auto Window::aspectRatio() const -> float
{
    return (float)m_width / (float)m_height;
}

void Window::clear_mouse_cursor_input()
{
    m_mouse_cursor_input = {};
};
