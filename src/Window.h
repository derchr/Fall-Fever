#pragma once

#include "input.h"

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

    Window();

    [[nodiscard]] auto glfw_window() -> GLFWwindow & { return *m_glfw_window; }
    [[nodiscard]] auto physical_dimensions() const -> glm::u32vec2;
    [[nodiscard]] auto logical_dimensions() const -> glm::u32vec2;
    [[nodiscard]] auto dimensions_changed() -> bool;
    [[nodiscard]] auto key_input() const -> auto const & { return m_key_input; }
    [[nodiscard]] auto mouse_cursor_input() const -> auto const & { return m_mouse_cursor_input; }
    [[nodiscard]] auto mouse_button_input() const -> auto const & { return m_mouse_button_input; }
    [[nodiscard]] auto cursor_catched() const -> auto { return m_mouse_catched; }

    void clear_mouse_cursor_input();
    void update_catched_mouse(entt::registry &registry) const;
    void update_descriptor(entt::registry &registry) const;

private:
    static void key_callback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods);
    static void mouse_cursor_callback(GLFWwindow *glfw_window, double xpos, double ypos);
    static void framebuffer_size_callback(GLFWwindow *glfw_window, int width, int height);
    static void glfw_error_callback(int error, const char *description);

    static constexpr float MOUSE_SENSITIVITY = 0.15F;

    void set_catched_cursor(bool value);

    std::shared_ptr<GLFWwindow> m_glfw_window;

    // <Key, Action>
    Input::Key m_key_input;
    Input::MouseButton m_mouse_button_input;
    Input::MouseCursor m_mouse_cursor_input;

    bool m_dimensions_changed = false;
    double m_last_cursor_pos_x = 0.0;
    double m_last_cursor_pos_y = 0.0;

    bool m_first_mouse_input = false;
    MouseCatched m_mouse_catched;
    bool m_wire_frame_mode = false;
};
