#pragma once

#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class Line : public ElementaryModel {
    public:
        Line(const glm::vec3& a, const glm::vec3& b);
        ~Line() override = default;

        Line(const Line&) = delete;
        Line& operator=(const Line&) = delete;

        Line(Line&&) noexcept = default;
        Line& operator=(Line&&) noexcept = default;
    };
}
