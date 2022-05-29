#pragma once

namespace Limitless {
    enum class VertexStreamUsage {
        Static,
        Dynamic,
        Stream
    };

    enum class VertexStreamDraw {
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdj = GL_TRIANGLE_STRIP_ADJACENCY,
        TriangleFan = GL_TRIANGLE_FAN,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        Points = GL_POINTS,
        Patches = GL_PATCHES,
        Quads = GL_QUADS
    };

    class AbstractVertexStream {
    public:
        AbstractVertexStream() = default;
        virtual ~AbstractVertexStream() = default;

        virtual void draw() noexcept = 0;
        virtual void draw(VertexStreamDraw draw) noexcept = 0;

        virtual void draw_instanced(std::size_t count) noexcept = 0;
        virtual void draw_instanced(VertexStreamDraw draw, std::size_t count) noexcept = 0;
    };
}
