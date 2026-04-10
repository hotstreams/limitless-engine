#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/functions.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <functional>

namespace Limitless {
    class Box {
    public:
        glm::vec3 center;
        glm::vec3 size;
    };

    template<typename V>
    Box calculateBoundingBox(const std::vector<V>& vertices) {
        auto min = glm::vec3{ std::numeric_limits<float>::max() };
        auto max = glm::vec3{ std::numeric_limits<float>::min() };

        for (const auto& v : vertices) {
            const glm::vec3 position = v.getPosition();
            min = glm::min(min, position);
            max = glm::max(max, position);
        }

        const auto center = (min + max) / 2.0f;
        const auto size = max - min;

        return { center, size };
    }

    inline Box calculateBoundingBox(std::function<void(std::function<void(const glm::vec3&)>)> for_each) {
        auto min = glm::vec3{ std::numeric_limits<float>::max() };
        auto max = glm::vec3{ std::numeric_limits<float>::min() };

        for_each([&min, &max](const glm::vec3& position){
            min = glm::min(min, position);
            max = glm::max(max, position);
        });

        const auto center = (min + max) / 2.0f;
        const auto size = max - min;

        return { center, size };
    }

    inline Box mergeBoundingBox(const Box& b1, const Box& b2) {
        auto min = glm::min(b1.center - b1.size / 2.0f, b2.center - b2.size / 2.0f);
        auto max = glm::max(b1.center + b1.size / 2.0f, b2.center + b2.size / 2.0f);

        const auto center = (min + max) / 2.0f;
        const auto size = max - min;

        return { center, size };
    }

    /**
     * Transforms a local axis-aligned box by a matrix and returns a world axis-aligned box.
     *
     * Uses the standard AABB transform:
     * - world_center = M * [local_center, 1]
     * - world_extents = abs(mat3(M)) * local_extents
     */
    inline Box transformBoundingBox(const Box& local, const glm::mat4& matrix) {
        const glm::vec3 local_extents = local.size * 0.5f;
        const glm::vec3 world_center = glm::vec3(matrix * glm::vec4(local.center, 1.0f));

        glm::mat3 m = glm::mat3(matrix);
        m[0] = glm::abs(m[0]);
        m[1] = glm::abs(m[1]);
        m[2] = glm::abs(m[2]);

        const glm::vec3 world_extents = m * local_extents;
        return { world_center, world_extents * 2.0f };
    }
}