#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/bounding_box.hpp>
#include <string>

namespace Limitless {
    enum class MeshDataType {
        Static,
        Dynamic,
        Stream
    };

    enum class DrawMode {
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdj = GL_TRIANGLE_STRIP_ADJACENCY,
        TriangleFan = GL_TRIANGLE_FAN,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        Points = GL_POINTS,
        Patches = GL_PATCHES
    };

    class AbstractMesh {
    public:
        AbstractMesh() = default;
        virtual ~AbstractMesh() = default;

        virtual void draw_instanced(size_t count) const noexcept = 0;
        virtual void draw() const noexcept = 0;
        virtual void draw(DrawMode mode) const noexcept = 0;

        [[nodiscard]] virtual const BoundingBox& getBoundingBox() noexcept = 0;
        [[nodiscard]] virtual const std::string& getName() const noexcept = 0;
        [[nodiscard]] virtual DrawMode getDrawMode() const noexcept = 0;
    };
}