#include "camera.h"
#include "util/Log.h"
#include "input.h"
#include "time.h"
#include "Window.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

auto Camera::projection_matrix() const -> glm::mat4
{
    return std::visit(
        [](auto &&arg) -> glm::mat4 {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Perspective>) {
                return glm::perspective(arg.fov / 2, arg.aspect_ratio, arg.near, arg.far);
            } else {
                return glm::ortho(arg.left, arg.right, arg.bottom, arg.top);
            }
        },
        projection);
};

auto Camera::view_matrix(GlobalTransform const &transform) -> glm::mat4
{
    return glm::lookAt(
        transform.position(), transform.position() + front_vector(transform), UP_VECTOR);
}

auto Camera::front_vector(GlobalTransform const &transform) -> glm::vec3
{
    return glm::normalize(transform.transform * glm::vec4(0.0, 0.0, 1.0, 0.0));
}

void Camera::keyboard_movement(entt::registry &registry)
{
    struct KeyboardMovementContext
    {
        bool accelerate{};
    };

    auto &movement_context = registry.ctx().emplace<KeyboardMovementContext>();
    auto const& key_input = registry.ctx().at<Input::Key>();
    auto const& delta_time = registry.ctx().at<Time::Delta>();

    auto camera_view = registry.view<Camera const, Transform, GlobalTransform const>();
    auto camera_entity = camera_view.front();
    auto [camera, camera_transform, camera_global_transform] = camera_view.get(camera_entity);

    glm::vec3 front_vec = front_vector(camera_global_transform);
    front_vec.y = 0; // NOLINT (cppcoreguidelines-pro-type-union-access)

    glm::vec3 delta_pos = glm::vec3(0., 0., 0.);
    float delta_factor = SPEED * delta_time.delta.count() * (movement_context.accelerate ? ACCELERATION : 1.0F);
    movement_context.accelerate = false;

    for (auto const &[key, pressed] : key_input.key_map) {
        if (key == GLFW_KEY_W && pressed) {
            delta_pos += delta_factor * glm::normalize(front_vec);
        }
        if (key == GLFW_KEY_S && pressed) {
            delta_pos -= delta_factor * glm::normalize(front_vec);
        }
        if (key == GLFW_KEY_A && pressed) {
            delta_pos -= delta_factor * glm::normalize(glm::cross(front_vec, Camera::UP_VECTOR));
        }
        if (key == GLFW_KEY_D && pressed) {
            delta_pos += delta_factor * glm::normalize(glm::cross(front_vec, Camera::UP_VECTOR));
        }
        if (key == GLFW_KEY_SPACE && pressed) {
            delta_pos += delta_factor * UP_VECTOR;
        }
        if (key == GLFW_KEY_LEFT_SHIFT && pressed) {
            delta_pos -= delta_factor * UP_VECTOR;
        }
        if (key == GLFW_KEY_LEFT_ALT && pressed) {
            movement_context.accelerate = true;
        }
    }
    camera_transform.translation += delta_pos;
}

void Camera::mouse_orientation(entt::registry &registry)
{
    auto camera_view = registry.view<Camera, Transform>();
    auto camera_entity = camera_view.front();
    auto [camera, camera_transform] = camera_view.get(camera_entity);

    auto const& mouse_cursor_input = registry.ctx().at<Input::MouseCursor>();
    auto [deltaX, deltaY] = mouse_cursor_input.cursor_movement;

    if (std::abs(deltaX) < std::numeric_limits<double>::epsilon() &&
        std::abs(deltaY) < std::numeric_limits<double>::epsilon()) {
        return;
    }

    auto pitch = static_cast<float>(deltaY);
    auto yaw = static_cast<float>(deltaX);
    auto roll = 0.0F;

    // Orthographic projection currently unsupported
    auto &camera_perspective = std::get<Perspective>(camera.projection);

    camera_perspective.pitch += glm::radians(pitch);
    camera_perspective.yaw += glm::radians(yaw);

    static constexpr float PITCH_CLIP = glm::radians(89.);
    camera_perspective.pitch =
        std::clamp(static_cast<float>(camera_perspective.pitch), -PITCH_CLIP, PITCH_CLIP);

    camera_transform.orientation =
        glm::quat(glm::vec3(-camera_perspective.pitch, -camera_perspective.yaw, 0.0));
}

void Camera::aspect_ratio_update(entt::registry &registry)
{
    float aspect_ratio = registry.ctx().at<Window::Descriptor>().aspect_ratio;

    auto camera_view = registry.view<Camera>();
    auto camera_entity = camera_view.front();
    auto [camera] = camera_view.get(camera_entity);

    // Orthographic projection currently unsupported
    auto &camera_perspective = std::get<Perspective>(camera.projection);
    camera_perspective.aspect_ratio = aspect_ratio;
}
