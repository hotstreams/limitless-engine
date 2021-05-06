#pragma once

namespace Limitless::fx {
    class SpriteEmitter;
    class MeshEmitter;
    class BeamEmitter;

    class EmitterVisitor {
    public:
        virtual void visit(const SpriteEmitter& emitter) noexcept = 0;
        virtual void visit(const MeshEmitter& emitter) noexcept = 0;
        virtual void visit(const BeamEmitter& emitter) noexcept = 0;

        virtual ~EmitterVisitor() = default;
    };
}