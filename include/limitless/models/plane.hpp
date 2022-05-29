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

    class PlaneQuad : public ElementaryModel {
    public:
        PlaneQuad();
        ~PlaneQuad() override = default;

        PlaneQuad(const PlaneQuad&) = delete;
        PlaneQuad& operator=(const PlaneQuad&) = delete;

        PlaneQuad(PlaneQuad&&) noexcept = default;
        PlaneQuad& operator=(PlaneQuad&&) noexcept = default;
    };
}