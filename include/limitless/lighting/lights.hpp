#pragma once

#include <glm/glm.hpp>

namespace LimitlessEngine {
    struct PointLight {
        static constexpr auto shader_storage_name = "point_light_buffer";

        glm::vec4 position {};
        glm::vec4 color {};
        float constant {1.0f};
        float linear {0.7f};
        float quadratic {1.8f};
        float radius {1.0f};

        PointLight() = default;

        PointLight(const glm::vec3& _position, const glm::vec4& _color, float _radius) noexcept
                : position{_position, 1.0f}, color{_color}, radius{_radius} {}
    };

    struct DirectionalLight {
        static constexpr auto shader_storage_name = "directional_light_buffer";

        glm::vec4 direction {};
        glm::vec4 color {};

        DirectionalLight(const glm::vec4& _direction, const glm::vec4& _color) noexcept
            : direction{_direction}, color{_color} {}
    };

    struct SpotLight {
        static constexpr auto shader_storage_name = "spot_light_buffer";

        static_assert(true, "TODO");
    };
}
