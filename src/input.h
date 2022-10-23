#pragma once

#include <entt/entt.hpp>
#include <unordered_map>
#include <utility>

namespace Input {

struct Key
{
    std::unordered_map<int, bool> key_map;
};

struct MouseButton
{
    std::unordered_map<int, bool> button_map;
};
struct MouseCursor
{
    std::pair<double, double> cursor_movement;
};

void handle_keyboard_input(entt::registry &registry, Key const &key_input);
void handle_mouse_button_input(entt::registry &registry, MouseButton const &mouse_button_input);
void handle_mouse_cursor_input(entt::registry &registry, MouseCursor const &mouse_cursor_input);

}; // namespace Input
