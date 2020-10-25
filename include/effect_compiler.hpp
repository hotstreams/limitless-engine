#pragma once

#include <material_compiler.hpp>

namespace GraphicsEngine {
    class Emitter;
    class SpriteEmitter;
    class MeshEmitter;
    class EffectInstance;

    class EffectCompiler : public MaterialCompiler {
    private:
        std::string getEmitterDefines(const Emitter& emitter) noexcept;
        void compile(const SpriteEmitter& emitter);
        void compile(const MeshEmitter& emitter);
    public:
        using ShaderCompiler::compile;
        void compile(const EffectInstance& instance);
    };
}