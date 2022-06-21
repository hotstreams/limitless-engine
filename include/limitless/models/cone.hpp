#pragma once

#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class Cone : public ElementaryModel {
    public:
        Cone();
        ~Cone() override = default;

        Cone(const Cone&) = delete;
        Cone& operator=(const Cone&) = delete;

        Cone(Cone&&) noexcept = default;
        Cone& operator=(Cone&&) noexcept = default;
    };
}