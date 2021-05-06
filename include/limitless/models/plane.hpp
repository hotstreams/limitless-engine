#pragma once

#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class Plane : public ElementaryModel {
    public:
        Plane();
        ~Plane() override = default;

        Plane(const Plane&) = delete;
        Plane& operator=(const Plane&) = delete;

        Plane(Plane&&) noexcept = default;
        Plane& operator=(Plane&&) noexcept = default;
    };
}