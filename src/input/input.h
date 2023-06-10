#pragma once

#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <set>
#include <utility>

namespace Input {

enum class KeyCode : int
{
};

enum class Action : int
{
};

struct KeyInput
{
    KeyCode key_code;
    Action action;
};

struct MouseMotion
{
    glm::vec2 delta{};
};

template <typename T> class State
{
public:
    auto pressed(T input) const -> bool { return pressed_keys.contains(input); }
    auto just_pressed(T input) const -> bool { return just_pressed_keys.contains(input); }
    auto just_released(T input) const -> bool { return just_pressed_keys.contains(input); }

    static void init_state(entt::registry& registry);
    static void update_state(entt::registry& registry);

private:
    void press(T input)
    {
        if (pressed_keys.insert(input).second) {
            just_pressed_keys.insert(input);
        }
    }

    void release(T input)
    {
        if (pressed_keys.erase(input) != 0) {
            just_released_keys.insert(input);
        }
    }

    std::set<T> pressed_keys;
    std::set<T> just_pressed_keys;
    std::set<T> just_released_keys;

    friend class KeyListener;
    friend class CursorListener;
};

struct KeyListener
{
    entt::registry& registry;
    void key_event(KeyInput const& key_input_event);
};

struct CursorListener
{
    entt::registry& registry;
    void cursor_event(MouseMotion const& mouse_motion_event);
};

void reset_mouse_motion(entt::registry& registry);

}; // namespace Input
