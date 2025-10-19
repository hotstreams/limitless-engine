#pragma once

#include <limitless/models/model.hpp>

namespace Limitless {
    // uses NDC space
    class Quad : public Model {
    public:
        Quad();
        ~Quad() override = default;

        Quad(const Quad&) = delete;
        Quad& operator=(const Quad&) = delete;

        Quad(Quad&&) noexcept = default;
        Quad& operator=(Quad&&) noexcept = default;
    };
}