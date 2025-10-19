#pragma once

#include <limitless/models/model.hpp>

namespace Limitless {
    class Plane : public Model {
    public:
        Plane();
        ~Plane() override = default;

        Plane(const Plane&) = delete;
        Plane& operator=(const Plane&) = delete;

        Plane(Plane&&) noexcept = default;
        Plane& operator=(Plane&&) noexcept = default;
    };

    /*
     * used only with tesselation pipeline
     */
    class PlaneQuad : public Model {
    public:
        PlaneQuad();
        ~PlaneQuad() override = default;

        PlaneQuad(const PlaneQuad&) = delete;
        PlaneQuad& operator=(const PlaneQuad&) = delete;

        PlaneQuad(PlaneQuad&&) noexcept = default;
        PlaneQuad& operator=(PlaneQuad&&) noexcept = default;
    };
}