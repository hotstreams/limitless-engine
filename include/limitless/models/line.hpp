#pragma once

#include <limitless/models/model.hpp>

#include "limitless/core/vertex.hpp"

namespace Limitless {
    class Line : public Model {
    public:
        Line(const glm::vec3& a, const glm::vec3& b);
        ~Line() override = default;

        Line(const Line&) = delete;
        Line& operator=(const Line&) = delete;

        Line(Line&&) noexcept = default;
        Line& operator=(Line&&) noexcept = default;
    };
}
