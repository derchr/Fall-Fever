#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <utility>

class GLFWwindow;

class Window
{
public:
    struct MouseCatched
    {
        bool catched = true;
    };

    struct Descriptor
    {
        glm::u32vec2 logical_dimensions;
        float aspect_ratio{};
    };

    struct ResizeEvent
    {};

    Window(entt::dispatcher& event_dispatcher);

    [[nodiscard]] auto handle() -> GLFWwindow& { return *glfw_window; }
    [[nodiscard]] auto physical_dimensions() const -> glm::u32vec2;
    [[nodiscard]] auto logical_dimensions() const -> glm::u32vec2;

    void update_descriptor(entt::registry& registry) const;
    void mouse_catching(entt::registry& registry) const;
    void close_on_esc(entt::registry& registry) const;

private:
    static void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    static void mouse_cursor_callback(GLFWwindow* glfw_window, double xpos, double ypos);
    static void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);
    static void glfw_error_callback(int error, char const* description);

    static constexpr float MOUSE_SENSITIVITY = 0.15F;

    std::shared_ptr<GLFWwindow> glfw_window;

    entt::dispatcher& event_dispatcher;

    glm::vec2 last_cursor_pos{};
    bool first_mouse_input = true;
};
