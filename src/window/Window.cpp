#include "Window.h"
#include "core/glad.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

static constexpr unsigned INIT_WINDOW_WIDTH = 1280;
static constexpr unsigned INIT_WINDOW_HEIGHT = 720;

Window::Window()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    // Maximize in release build
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#endif

    m_glfw_window = std::shared_ptr<GLFWwindow>(
        glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "OpenGL", nullptr, nullptr),
        [](GLFWwindow* window) { glfwDestroyWindow(window); });

    if (!m_glfw_window) {
        spdlog::critical("Failed to create window");
    }

    // Create OpenGL context
    glfwMakeContextCurrent(m_glfw_window.get());

#ifndef NDEBUG
    // Disable mouse cursor
    m_mouse_catched.catched = false;
#endif

    set_catched_cursor(m_mouse_catched.catched);

    // Callbacks
    glfwSetWindowUserPointer(m_glfw_window.get(), this);
    glfwSetKeyCallback(m_glfw_window.get(), key_callback);
    glfwSetCursorPosCallback(m_glfw_window.get(), mouse_cursor_callback);
    glfwSetFramebufferSizeCallback(m_glfw_window.get(), framebuffer_size_callback);

    init_glad();
}

auto Window::dimensions_changed() -> bool
{
    bool temp = m_dimensions_changed;
    m_dimensions_changed = false;
    return temp;
}

void Window::set_catched_cursor(bool value)
{
    glfwSetInputMode(
        m_glfw_window.get(), GLFW_CURSOR, value ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    m_mouse_catched.catched = value;
}

void Window::glfw_error_callback(int error, char const* description)
{
    spdlog::warn("GLFW [{:d}]: {:s}\n", error, description);
}

void Window::framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height)
{
    auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    window.m_dimensions_changed = true;
    glViewport(0, 0, width, height);
}

void Window::key_callback(GLFWwindow* glfw_window,
                          int key,
                          [[maybe_unused]] int scancode,
                          int action,
                          [[maybe_unused]] int mods)
{
    auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
    } else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        window.set_catched_cursor(!window.m_mouse_catched.catched);
    } else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        window.m_wire_frame_mode = !window.m_wire_frame_mode;
        glPolygonMode(GL_FRONT_AND_BACK, window.m_wire_frame_mode ? GL_LINE : GL_FILL);
    } else if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        window.m_key_input.key_map[key] = (action == GLFW_PRESS);
    }
}

void Window::mouse_cursor_callback(GLFWwindow* glfw_window, double xpos, double ypos)
{
    auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

    double deltaCursorPosX = xpos - window.m_last_cursor_pos_x;
    double deltaCursorPosY = -(ypos - window.m_last_cursor_pos_y);

    window.m_last_cursor_pos_x = xpos;
    window.m_last_cursor_pos_y = ypos;

    // Check if this is the first VALID mouse event after window being resized
    if (window.m_first_mouse_input &&
        (std::abs(deltaCursorPosX) >= std::numeric_limits<double>::epsilon() ||
         std::abs(deltaCursorPosY) >= std::numeric_limits<double>::epsilon())) {
        window.m_first_mouse_input = false;
        deltaCursorPosX = 0.0;
        deltaCursorPosY = 0.0;
    }

    deltaCursorPosX *= MOUSE_SENSITIVITY;
    deltaCursorPosY *= MOUSE_SENSITIVITY;

    auto& [deltaX, deltaY] = window.m_mouse_cursor_input.cursor_movement;
    deltaX += deltaCursorPosX;
    deltaY += deltaCursorPosY;
}

auto Window::logical_dimensions() const -> glm::u32vec2
{
    int width{};
    int height{};
    glfwGetWindowSize(m_glfw_window.get(), &width, &height);
    return {width, height};
}

auto Window::physical_dimensions() const -> glm::u32vec2
{
    int width{};
    int height{};
    glfwGetFramebufferSize(m_glfw_window.get(), &width, &height);
    return {width, height};
}

void Window::clear_mouse_cursor_input()
{
    m_mouse_cursor_input = {};
};

void Window::update_catched_mouse(entt::registry& registry) const
{
    registry.ctx().erase<MouseCatched>();
    registry.ctx().emplace<MouseCatched>(m_mouse_catched);
}

void Window::update_descriptor(entt::registry& registry) const
{
    auto dimensions = logical_dimensions();

    registry.ctx().erase<Descriptor>();
    registry.ctx().emplace<Descriptor>(Descriptor{
        .logical_dimensions = dimensions,
        .aspect_ratio = static_cast<float>(dimensions.x) / static_cast<float>(dimensions.y)});
}
