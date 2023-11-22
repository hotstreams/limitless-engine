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
            , color {_color} {
        }
    };

    struct Light final {
        static constexpr auto SHADER_STORAGE_NAME = "LIGHTS_BUFFER";

        glm::vec4 color {};
        glm::vec4 position {};
        glm::vec4 direction {};
        glm::vec2 scale_offset {};
        float falloff {};
        uint32_t type {1u};

        Light() = default;
        ~Light() = default;

        auto getRadius() const {
            return sqrt(1.0 / falloff);
        }

        Light(const glm::vec3& position, const glm::vec4& _color, const float radius) noexcept
                : position {position, 1.0}
                , color {_color}
                , falloff {1.0f / (radius * radius)} {
        }
    };
}
