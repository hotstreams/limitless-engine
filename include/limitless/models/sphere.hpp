#pragma once

#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class Sphere : public ElementaryModel {
    public:
        Sphere(uint32_t x_segments, uint32_t y_segments);
        ~Sphere() override = default;

        Sphere(const Sphere &) = delete;
        Sphere &operator=(const Sphere &) = delete;

        Sphere(Sphere&&) noexcept = default;
        Sphere& operator=(Sphere&&) noexcept = default;
    };
}