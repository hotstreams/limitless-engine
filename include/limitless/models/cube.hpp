#pragma once

#include <limitless/models/elementary_model.hpp>

namespace LimitlessEngine {
    class Cube : public ElementaryModel {
    public:
        Cube();
        ~Cube() override = default;

        Cube(const Cube&) = delete;
        Cube& operator=(const Cube&) = delete;

        Cube(Cube&&) noexcept = default;
        Cube& operator=(Cube&&) noexcept = default;
    };
}