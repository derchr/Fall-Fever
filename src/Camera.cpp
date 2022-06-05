#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspectRatio) : m_fov(fov)
{
    m_viewMatrix = glm::mat4(1.);
    updateAspectRatio(aspectRatio);
    updateVPM();
}

void Camera::updateVPM()
{
    m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}

void Camera::updateAspectRatio(float aspectRatio)
{
    // m_projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.f, 100.0f);
    m_projectionMatrix = glm::perspective(m_fov / 2.F, aspectRatio, .1F, 1000.F);
    updateVPM();
}

void Camera::translate(glm::vec3 translateVector)
{
    m_position += translateVector;
    m_viewMatrix = glm::translate(m_viewMatrix, translateVector * -1.F);
}

void Camera::lookAtTarget(glm::vec3 target)
{
    m_viewMatrix = glm::lookAt(m_position, target, UP_VEC);
}

void Camera::lookForward()
{
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front_vec, UP_VEC);
}

void Camera::updatePositionFromKeyboardInput(KeyInput const &key_input, float deltaTime)
{
    glm::vec3 frontVecWithoutY = glm::vec3(m_front_vec.x, 0., m_front_vec.z);
    glm::vec3 deltaPos = glm::vec3(0., 0., 0.);

    for (auto const &[key, pressed] : key_input) {
        if (key == GLFW_KEY_W && pressed) {
            deltaPos += SPEED * deltaTime * glm::normalize(frontVecWithoutY);
        }
        if (key == GLFW_KEY_S && pressed) {
            deltaPos -= SPEED * deltaTime * glm::normalize(frontVecWithoutY);
        }
        if (key == GLFW_KEY_A && pressed) {
            deltaPos -= SPEED * deltaTime * glm::normalize(glm::cross(m_front_vec, UP_VEC));
        }
        if (key == GLFW_KEY_D && pressed) {
            deltaPos += SPEED * deltaTime * glm::normalize(glm::cross(m_front_vec, UP_VEC));
        }
        if (key == GLFW_KEY_SPACE && pressed) {
            deltaPos += SPEED * deltaTime * UP_VEC;
        }
        if (key == GLFW_KEY_LEFT_SHIFT && pressed) {
            deltaPos -= SPEED * deltaTime * UP_VEC;
        }
    }
    m_position += deltaPos;
}

void Camera::updateDirectionFromMouseInput(MouseCursorInput const &mouse_cursor_input)
{
    auto [deltaX, deltaY] = mouse_cursor_input;

    if (deltaX == 0 && deltaY == 0) {
        return;
    }

    m_yaw += deltaX;
    m_pitch += deltaY;

    static constexpr float CLIP = 89.;

    if (m_pitch > CLIP) {
        m_pitch = CLIP;
    }
    if (m_pitch < -CLIP) {
        m_pitch = -CLIP;
    }

    glm::vec3 direction;
    direction.x = std::cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = std::sin(glm::radians(m_pitch));
    direction.z = std::sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front_vec = glm::normalize(direction);
}

void Camera::setPosition(glm::vec3 position)
{
    this->m_position = position;
}

auto Camera::getView() const -> glm::mat4
{
    return m_viewMatrix;
}

auto Camera::getProj() const -> glm::mat4
{
    return m_projectionMatrix;
}

auto Camera::getViewProj() const -> glm::mat4
{
    return m_viewProjectionMatrix;
}

auto Camera::getPosition() const -> glm::vec3
{
    return m_position;
}

auto Camera::getDirection() const -> glm::vec3
{
    return m_front_vec;
}
