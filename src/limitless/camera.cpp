#include <limitless/camera.hpp>
#include <algorithm>

using namespace Limitless;

Camera::Camera(glm::uvec2 screen_size) noexcept
    : position{0.0f}
    , front {1.0f, 0.0f, 0.0f}
    , up {0.0f, 1.0f, 0.0f}
    , right {0.0f, 0.0f, 1.0f}
    , world_up {0.0f, 1.0f, 0.0f}
    , projection {1.0f}
    , view {1.0f}
    , pitch {-60.0f}
    , yaw {270.0f}
{
    updateView();
    updateProjection(screen_size);
}

void Camera::setPitch(float _pitch) noexcept {
    pitch = std::clamp(_pitch, -89.0f, 89.0f);
    updateView();
}

void Camera::setYaw(float _yaw) noexcept {
    yaw = std::fmod(_yaw, 360.0f);
    updateView();
}

void Camera::setRotation(float _pitch, float _yaw) noexcept {
    pitch = std::clamp(_pitch, -89.0f, 89.0f);
    yaw = std::fmod(_yaw, 360.0f);
    updateView();
}

void Camera::updateView() noexcept {
    // quaternion
    // TODO: slepr between quats
//    glm::quat pitch_quat = glm::angleAxis(glm::radians(pitch), right);
//    glm::quat heading_quat = glm::angleAxis(glm::radians(yaw), up);
//    front = glm::normalize(pitch_quat * front * heading_quat);

    // euler angles
    front.x = glm::cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));

    view = glm::lookAt(position, position + front, up);
}

void Camera::updateProjection(glm::uvec2 screen_size) noexcept {
    projection = glm::perspective(
        glm::radians(fov),
        static_cast<float>(screen_size.x) / static_cast<float>(screen_size.y),
        near_distance,
        far_distance
    );

    // matrix that converts from
    // clip space [-1, 1] to screen space [0, screen size]
    glm::mat4 clip_to_screen = glm::mat4(
        0.5 * screen_size.x, 0.0, 0.0, 0.0,
        0.0, 0.5 * screen_size.y, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.5 * screen_size.x, 0.5 * screen_size.y, 0.0, 1.0
    );

    view_to_screen = clip_to_screen * projection;
}

void Camera::setPosition(const glm::vec3& _position) noexcept {
    position = _position;

    updateView();
}

void Camera::setFov(glm::uvec2 screen_size, float _fov) noexcept {
	fov = _fov;
	updateProjection(screen_size);
}
