#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

using KeyInput = std::unordered_map<int, bool>;
using MouseButtonInput = std::unordered_map<int, bool>;
using MouseCursorInput = std::pair<double, double>;

class GLFWwindow;

class Window
{
public:
    Window();

    [[nodiscard]] auto glfw_window() -> GLFWwindow & { return *m_glfw_window; }
    [[nodiscard]] auto dimensions() const -> std::pair<unsigned, unsigned>;
    [[nodiscard]] auto dimensions_changed() const -> bool;
    [[nodiscard]] auto aspectRatio() const -> float;
    [[nodiscard]] auto key_input() const -> auto const & { return m_key_input; }
    [[nodiscard]] auto mouse_cursor_input() const -> auto const & { return m_mouse_cursor_input; }
    [[nodiscard]] auto mouse_button_input() const -> auto const & { return m_mouse_button_input; }
    [[nodiscard]] auto cursor_catched() const -> auto { return m_mouse_catched; }

    void clear_mouse_cursor_input();
    void update_dimensions();

private:
    static void key_callback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow *glfw_window, int button, int action, int mods);
    static void mouse_cursor_callback(GLFWwindow *glfw_window, double xpos, double ypos);

    static void glfw_error_callback(int error, const char *description);
    static void framebuffer_size_callback(GLFWwindow *glfw_window, int width, int height);

    static constexpr float MOUSE_SENSITIVITY = 0.15;

    void set_catched_cursor(bool value);

    std::shared_ptr<GLFWwindow> m_glfw_window;

    // <Key, Action>
    KeyInput m_key_input;
    MouseButtonInput m_mouse_button_input;
    MouseCursorInput m_mouse_cursor_input;

    int m_posX{};
    int m_posY{};
    int m_width{};
    int m_height{};
    double m_last_cursor_pos_x = 0.0;
    double m_last_cursor_pos_y = 0.0;

    bool m_first_mouse_input = false;
    bool m_mouse_catched = true;
    bool m_wire_frame_mode = false;
};
