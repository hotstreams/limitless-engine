#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Limitless {
    class Camera {
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;

        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 view_to_screen;

        float pitch; // degrees
        float yaw;

        float fov {90}; // degrees
        float near_distance {0.001f};
        float far_distance {1000.0f};

    public:
        explicit Camera(glm::uvec2 screen_size) noexcept;

        [[nodiscard]] const auto& getViewToScreen() const noexcept { return view_to_screen; }
        [[nodiscard]] const auto& getProjection() const noexcept { return projection; }
        [[nodiscard]] const auto& getPosition() const noexcept { return position; }
        [[nodiscard]] const auto& getFront() const noexcept { return front; }
        [[nodiscard]] const auto& getUp() const noexcept { return up; }
        [[nodiscard]] const auto& getRight() const noexcept { return right; }
        [[nodiscard]] const auto& getView() const noexcept { return view; }
        [[nodiscard]] const auto& getNear() const noexcept { return near_distance; }
        [[nodiscard]] const auto& getFar() const noexcept { return far_distance; }
        [[nodiscard]] const auto& getFov() const noexcept { return fov; }
        [[nodiscard]] const auto& getPitch() const noexcept { return pitch; }
        [[nodiscard]] const auto& getYaw() const noexcept { return yaw; }

        void setPosition(const glm::vec3& position) noexcept;
        void setFov(glm::uvec2 screen_size, float fov) noexcept;
        void setPitch(float pitch) noexcept;
        void setYaw(float yaw) noexcept;
        void setRotation(float pitch, float yaw) noexcept;

        void updateView() noexcept;
        void updateProjection(glm::uvec2 screen_size) noexcept;
    };
}
