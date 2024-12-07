#include "window.h"
#include "core/glad.h"
#include "input/input.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

static constexpr unsigned INIT_WINDOW_WIDTH = 1280;
static constexpr unsigned INIT_WINDOW_HEIGHT = 720;

Window::Window(entt::dispatcher& event_dispatcher) : event_dispatcher(event_dispatcher)
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

    glfw_window = std::shared_ptr<GLFWwindow>(
        glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "OpenGL", nullptr, nullptr),
        [](GLFWwindow* window) { glfwDestroyWindow(window); });

    if (!glfw_window) {
        spdlog::critical("Failed to create window");
    }

    // Create OpenGL context
    glfwMakeContextCurrent(glfw_window.get());

    // Callbacks
    glfwSetWindowUserPointer(glfw_window.get(), this);
    glfwSetKeyCallback(glfw_window.get(), key_callback);
    glfwSetCursorPosCallback(glfw_window.get(), mouse_cursor_callback);
    glfwSetFramebufferSizeCallback(glfw_window.get(), framebuffer_size_callback);

    init_glad();
    
    {
        int width, height;
        glfwGetFramebufferSize(glfw_window.get(), &width, &height);
        glViewport(0, 0, width, height);
    }
}

void Window::glfw_error_callback(int error, char const* description)
{
    spdlog::warn("GLFW [{:d}]: {:s}\n", error, description);
}

void Window::framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height)
{
    auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
    glViewport(0, 0, width, height);

    window.event_dispatcher.enqueue<ResizeEvent>();
}

void Window::key_callback(GLFWwindow* glfw_window,
                          int key,
                          [[maybe_unused]] int scancode,
                          int action,
                          [[maybe_unused]] int mods)
{
    auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

    window.event_dispatcher.enqueue<Input::KeyInput>(
        Input::KeyInput{.key_code = static_cast<Input::KeyCode>(key),
                        .action = static_cast<Input::Action>(action)});
}

void Window::mouse_cursor_callback(GLFWwindow* glfw_window, double xpos, double ypos)
{
    auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

    glm::vec2 delta{xpos - window.last_cursor_pos.x, ypos - window.last_cursor_pos.y};

    window.last_cursor_pos = {xpos, ypos};
    delta *= MOUSE_SENSITIVITY;

    // Check if this is the first VALID mouse event after window being resized
    if (window.first_mouse_input && (delta.x >= std::numeric_limits<double>::epsilon() ||
                                     delta.y >= std::numeric_limits<double>::epsilon())) {
        window.first_mouse_input = false;
        return;
    }

    window.event_dispatcher.enqueue<Input::MouseMotion>(Input::MouseMotion{.delta = delta});
}

auto Window::logical_dimensions() const -> glm::u32vec2
{
    int width{};
    int height{};
    glfwGetWindowSize(glfw_window.get(), &width, &height);
    return {width, height};
}

auto Window::physical_dimensions() const -> glm::u32vec2
{
    int width{};
    int height{};
    glfwGetFramebufferSize(glfw_window.get(), &width, &height);
    return {width, height};
}

void Window::mouse_catching(entt::registry& registry) const
{
    auto& mouse_catched = registry.ctx().emplace<MouseCatched>(MouseCatched{.catched = false});
    auto const& key_state = registry.ctx().get<Input::State<Input::KeyCode>>();

    if (key_state.just_pressed(Input::KeyCode{GLFW_KEY_LEFT_CONTROL})) {
        mouse_catched.catched = !mouse_catched.catched;

        glfwSetInputMode(glfw_window.get(),
                         GLFW_CURSOR,
                         mouse_catched.catched ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

void Window::close_on_esc(entt::registry& registry) const
{
    auto const& key_state = registry.ctx().get<Input::State<Input::KeyCode>>();

    if (key_state.just_pressed(Input::KeyCode{GLFW_KEY_ESCAPE})) {
        glfwSetWindowShouldClose(glfw_window.get(), GLFW_TRUE);
    }
}

void Window::update_descriptor(entt::registry& registry) const
{
    auto dimensions = logical_dimensions();

    registry.ctx().erase<Descriptor>();
    registry.ctx().emplace<Descriptor>(Descriptor{
        .logical_dimensions = dimensions,
        .aspect_ratio = static_cast<float>(dimensions.x) / static_cast<float>(dimensions.y)});
}
