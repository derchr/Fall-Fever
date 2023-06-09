#include "input.h"

void Input::handle_keyboard_input(entt::registry &registry, Key const &key_input)
{
    registry.ctx().erase<Key>();
    registry.ctx().emplace<Key>(key_input);
}

void Input::handle_mouse_button_input(entt::registry &registry,
                                      MouseButton const &mouse_button_input)
{
    registry.ctx().erase<MouseButton>();
    registry.ctx().emplace<MouseButton>(mouse_button_input);
}

void Input::handle_mouse_cursor_input(entt::registry &registry,
                                      MouseCursor const &mouse_cursor_input)
{
    registry.ctx().erase<MouseCursor>();
    registry.ctx().emplace<MouseCursor>(mouse_cursor_input);
}
