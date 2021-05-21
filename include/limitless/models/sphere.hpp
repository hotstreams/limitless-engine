#pragma once

#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class Sphere : public ElementaryModel {
    public:
        Sphere(glm::uvec2 segment_count);
        ~Sphere() override = default;

        Sphere(const Sphere &) = delete;
        Sphere &operator=(const Sphere &) = delete;

        Sphere(Sphere&&) noexcept = default;
        Sphere& operator=(Sphere&&) noexcept = default;
    };
}