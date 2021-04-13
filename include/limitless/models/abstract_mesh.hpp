#pragma once

#include <limitless/util/bounding_box.hpp>
#include <string>

namespace LimitlessEngine {
    class AbstractMesh {
    public:
        AbstractMesh() = default;
        virtual ~AbstractMesh() = default;

        virtual void draw_instanced(size_t count) const noexcept = 0;
        virtual void draw() const noexcept = 0;

        [[nodiscard]] virtual const BoundingBox& getBoundingBox() noexcept = 0;
        [[nodiscard]] virtual const std::string& getName() const noexcept = 0;
    };
}