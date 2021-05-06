#include <limitless/camera.hpp>
#include <iostream>

using namespace Limitless;

Camera::Camera(glm::uvec2 window_size) noexcept
    : position{0.0f}, front{1.0f, 0.0f, 0.0f}, up{0.0f, 1.0f, 0.0f}, right{0.0f, 0.0f, 1.0f}, world_up{0.0f, 1.0f, 0.0f},
    projection{1.0f}, view{1.0f}, pitch{-60.0f}, yaw{90.0f} {

    updateView();
    updateProjection(window_size);
}

void Camera::mouseMove(glm::dvec2 offset) noexcept {
    switch (mode) {
        case CameraMode::Free:
            offset *= mouse_sence;

            yaw += offset.x;
            pitch += offset.y;

            pitch = pitch >= 89.0f ? 89.0f : pitch;
            pitch = pitch <= -89.0f ? -89.0f : pitch;
            break;
        case CameraMode::Panning:
            break;
    }

    updateView();
}

void Camera::mouseScroll(float yoffset) noexcept {
    fov -= yoffset;

    fov = fov >= 150.0f ? 150.0f : fov;
    fov = fov <= 1.0f ? 1.0f : fov;
}


void Camera::movement(CameraMovement move, float delta) noexcept {
    auto velocity = move_speed * delta;

    switch (mode) {
        case CameraMode::Free:
            switch (move) {
                case CameraMovement::Forward:
                    position += front * velocity;
                    break;
                case CameraMovement::Backward:
                    position -= front * velocity;
                    break;
                case CameraMovement::Left:
                    position -= right * velocity;
                    break;
                case CameraMovement::Right:
                    position += right * velocity;
                    break;
                case CameraMovement::Up:
                    position += world_up * velocity;
                    break;
                case CameraMovement::Down:
                    position -= world_up * velocity;
                    break;
            }
            break;
        case CameraMode::Panning:
            switch (move) {
                case CameraMovement::Forward:
                    position.z += velocity;
                    break;
                case CameraMovement::Backward:
                    position.z -= velocity;
                    break;
                case CameraMovement::Left:
                    position.x += velocity;
                    break;
                case CameraMovement::Right:
                    position.x -= velocity;
                    break;
                case CameraMovement::Up:
                    position.y += velocity;
                    break;
                case CameraMovement::Down:
                    position.y -= velocity;
                    break;
            }
            break;
    }

    updateView();
}

void Camera::updateView() noexcept {
    // quaternion
    // TODO: slepr between quats
//    glm::quat pitch_quat = glm::angleAxis(glm::radians(pitch), right);
//    glm::quat heading_quat = glm::angleAxis(glm::radians(yaw), up);
//    front = glm::normalize(pitch_quat * front * heading_quat);

    // euler angles
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));

    view = glm::lookAt(position, position + front, up);
}

void Camera::updateProjection(glm::uvec2 size) noexcept {
    projection = glm::perspective(glm::radians(fov), static_cast<float>(size.x) / static_cast<float>(size.y), near_distance, far_distance);
}

void Camera::setPosition(const glm::vec3& _position) noexcept {
    position = _position;

    updateView();
}

void Camera::setFront(const glm::vec3& _front) noexcept {
    front = _front;

    updateView();
}

void Camera::setFov(glm::uvec2 size, float _fov) noexcept {
	fov = _fov;
	updateProjection(size);
}

