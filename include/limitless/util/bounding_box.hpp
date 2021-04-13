#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace LimitlessEngine {
    struct BoundingBox {
        glm::vec3 center;
        glm::vec3 size;
    };

    template<typename V>
    inline BoundingBox calculateBoundingBox(const std::vector<V>& vertices) {
        static_assert(sizeof(V::position) != 0);

        auto min = glm::vec3{ std::numeric_limits<float>::max() };
        auto max = glm::vec3{ std::numeric_limits<float>::min() };

        for (const auto& v : vertices) {
            min = glm::min(min, v.position);
            max = glm::max(max, v.position);
        }

        const auto center = (min + max) / 2.0f;
        const auto size = max - min;

        return { center, size };
    }

    inline BoundingBox mergeBoundingBox(const BoundingBox& b1, const BoundingBox& b2) {
        auto min = glm::min(b1.center - b1.size / 2.0f, b2.center - b2.size / 2.0f);
        auto max = glm::max(b1.center + b1.size / 2.0f, b2.center + b2.size / 2.0f);

        const auto center = (min + max) / 2.0f;
        const auto size = max - min;

        return { center, size };
    }
}