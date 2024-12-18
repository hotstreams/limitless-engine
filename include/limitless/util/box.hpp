#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/functions.hpp>
#include <vector>

namespace Limitless {
    class Box {
    public:
        glm::vec3 center;
        glm::vec3 size;
    };

    template<typename V>
    inline Box calculateBoundingBox(const std::vector<V>& vertices) {
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

    inline Box mergeBoundingBox(const Box& b1, const Box& b2) {
        auto min = glm::min(b1.center - b1.size / 2.0f, b2.center - b2.size / 2.0f);
        auto max = glm::max(b1.center + b1.size / 2.0f, b2.center + b2.size / 2.0f);

        const auto center = (min + max) / 2.0f;
        const auto size = max - min;

        return { center, size };
    }
}