#include "camera.h"
#include "window/window.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

auto Camera::projection_matrix() const -> glm::mat4
{
    return std::visit(
        [](auto&& arg) -> glm::mat4 {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Perspective>) {
                return glm::perspective(arg.fov / 2, arg.aspect_ratio, arg.near, arg.far);
            } else {
                return glm::ortho(arg.left, arg.right, arg.bottom, arg.top);
            }
        },
        projection);
};

auto Camera::view_matrix(GlobalTransform const& transform) -> glm::mat4
{
    return glm::lookAt(
        transform.position(), transform.position() + front_vector(transform), UP_VECTOR);
}

auto Camera::front_vector(GlobalTransform const& transform) -> glm::vec3
{
    return glm::normalize(transform.transform * glm::vec4(0.0, 0.0, 1.0, 0.0));
}

void Camera::aspect_ratio_update(entt::registry& registry)
{
    float aspect_ratio = registry.ctx().get<Window::Descriptor>().aspect_ratio;

    auto camera_view = registry.view<Camera>();

    for (auto [entity, camera] : camera_view.each()) {
        // Orthographic projection currently unsupported
        auto& camera_perspective = std::get<Perspective>(camera.projection);
        camera_perspective.aspect_ratio = aspect_ratio;
    }
}
