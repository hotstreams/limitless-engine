#pragma once

#include <glm/glm.hpp>

namespace Limitless {
    struct DirectionalLight final {
        glm::vec4 direction {};
        glm::vec4 color {};

        DirectionalLight() = default;
        ~DirectionalLight() = default;

        DirectionalLight(const glm::vec4& _direction, const glm::vec4& _color) noexcept
            : direction {_direction}
            , color {_color} {}
    };

    struct PointLight final {
        static constexpr auto SHADER_STORAGE_NAME = "POINT_LIGHTS_BUFFER";

        glm::vec4 position {};
        glm::vec4 color {};
        float constant {1.0f};
        float linear {0.7f};
        float quadratic {1.8f};
        float radius {1.0f};

        PointLight() = default;
        ~PointLight() = default;

        PointLight(const glm::vec3& _position, const glm::vec4& _color, float _radius) noexcept
            : position {_position, 1.0f}
            , color {_color}
            , radius {_radius} {}
    };

    struct SpotLight final {
        static constexpr auto SHADER_STORAGE_NAME = "SPOT_LIGHTS_BUFFER";

        glm::vec4 position {};
        glm::vec4 direction {};
        glm::vec4 color {};
        float cutoff {};

        SpotLight() = default;
        ~SpotLight() = default;

        SpotLight(const glm::vec3& _position, const glm::vec3& _direction, const glm::vec4& _color, float _cutoff) noexcept
            : position {_position, 1.0f}
            , direction {_direction, 1.0f}
            , color {_color}
            , cutoff {_cutoff} {}
    };
}
