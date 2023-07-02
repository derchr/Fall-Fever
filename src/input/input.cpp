#include "input.h"

#include <GLFW/glfw3.h>

namespace Input {

void KeyListener::key_event(KeyInput const& key_input_event)
{
    auto& key_state = registry.ctx().get<State<KeyCode>>();

    if (key_input_event.action == static_cast<Action>(GLFW_PRESS)) {
        key_state.press(key_input_event.key_code);
    } else if (key_input_event.action == static_cast<Action>(GLFW_RELEASE)) {
        key_state.release(key_input_event.key_code);
    }
}

void CursorListener::cursor_event(MouseMotion const& mouse_motion_event)
{
    auto& mouse_motion = registry.ctx().get<MouseMotion>();
    mouse_motion.delta += mouse_motion_event.delta;
}

void reset_mouse_motion(entt::registry& registry)
{
    auto& mouse_motion = registry.ctx().get<MouseMotion>();
    mouse_motion = {};
}

template <typename T> void State<T>::update_state(entt::registry& registry)
{
    auto& state = registry.ctx().get<State<T>>();
    state.just_pressed_keys.clear();
    state.just_released_keys.clear();
}

template class State<KeyCode>;

} // namespace Input
