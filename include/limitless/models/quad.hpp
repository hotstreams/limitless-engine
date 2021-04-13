#pragma once

#include <limitless/models/elementary_model.hpp>

namespace LimitlessEngine {
    // uses NDC space
    class Quad : public ElementaryModel {
    public:
        Quad();
        ~Quad() override = default;

        Quad(const Quad&) = delete;
        Quad& operator=(const Quad&) = delete;

        Quad(Quad&&) noexcept = default;
        Quad& operator=(Quad&&) noexcept = default;
    };
}