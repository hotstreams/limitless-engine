#pragma once

#include <glm/glm.hpp>

namespace Limitless {
    struct Vertex {
        glm::vec3 position;
        glm::vec2 uv;

        auto& getPosition() noexcept { return position; }
        const auto& getPosition() const noexcept { return position; }
    };

    struct TextVertex {
        glm::vec2 position;
        glm::vec2 uv;

        TextVertex(glm::vec2 _position, glm::vec2 _uv) noexcept
            : position{_position}
            , uv{_uv} {}

        TextVertex() = default;
    };

    struct VertexNormal {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        auto& getPosition() noexcept { return position; }
        const auto& getPosition() const noexcept { return position; }
    };

    struct VertexNormalTangent {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 uv;

        auto& getPosition() noexcept { return position; }
        const auto& getPosition() const noexcept { return position; }
    };

    struct VertexTerrain {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 uv;
        glm::vec2 uv1;
        glm::vec2 uv2;
        glm::vec2 uv3;
        uint32_t current;
        uint32_t mask;
        uint32_t types;

        auto& getPosition() noexcept { return position; }
        const auto& getPosition() const noexcept { return position; }
    };

    struct VertexPackedNormalTangent {
        glm::vec3 position;
        uint32_t normal;
        uint32_t tangent;
        uint32_t uv;
    };

    inline uint32_t pack(const glm::vec3& value) {
        const uint32_t xs = value.x < 0;
        const uint32_t ys = value.y < 0;
        const uint32_t zs = value.z < 0;
        const uint32_t ws = 0;
        uint32_t vi =
                ws << 31 | (uint32_t)(0) << 30 |
                zs << 29 | ((uint32_t)(value.z * 511 + (zs << 9)) & 511) << 20 |
                ys << 19 | ((uint32_t)(value.y * 511 + (ys << 9)) & 511) << 10 |
                xs << 9  | ((uint32_t)(value.x * 511 + (xs << 9)) & 511);
        return vi;
    }
}