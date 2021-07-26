#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/bounding_box.hpp>
#include <string>
#include <limitless/core/abstract_vertex_stream.hpp>

namespace Limitless {
    class AbstractMesh : public AbstractVertexStream {
    public:
        AbstractMesh() = default;
        ~AbstractMesh() override = default;

        [[nodiscard]] virtual const BoundingBox& getBoundingBox() noexcept = 0;
        [[nodiscard]] virtual const std::string& getName() const noexcept = 0;
        [[nodiscard]] virtual std::string& getName() noexcept = 0;
    };
}