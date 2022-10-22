#include "camera.h"
#include "util/Log.h"
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
