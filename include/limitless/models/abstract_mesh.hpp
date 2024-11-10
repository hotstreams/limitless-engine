#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/box.hpp>
#include <string>
#include <limitless/core/abstract_vertex_stream.hpp>

namespace Limitless {
    /**
     *      mesh
     *
     *
     *      set lod level for shadow?
     *
     *      lods
     *
     *      DrawElements
     *      MultiDrawElements
     *      MultiDrawElementsIndirect
     *
     *
     *      batched_mesh
     *
     *
     *      indirect batched mesh ?
     *
     *      indirect mesh ?
     *
     *      batched instance ?
     *
     *      indirect batched instance ?
     *
     *      batched level ? instance/mesh
     *
     *
     *
     *      vertex stream
     *
     *      stream[STREAM.POSITION]
     *      stream[STREAM.NORMAL]
     *      stream[STREAM.UV]
     *      stream[STREAM.COLOR]
     *      stream[STREAM.BONES]
     *      stream[STREAM.WEIGHTS]
     *      stream[STREAM.TANGENT]
     *
     *      stream[STREAM.INDEX]
     *
     */

    class AbstractMesh : public AbstractVertexStream {
    public:
        AbstractMesh() = default;
        ~AbstractMesh() override = default;

        [[nodiscard]] virtual const Box& getBoundingBox() noexcept = 0;
        [[nodiscard]] virtual const std::string& getName() const noexcept = 0;
        [[nodiscard]] virtual std::string& getName() noexcept = 0;
    };
}