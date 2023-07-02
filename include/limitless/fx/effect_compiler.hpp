#pragma once

#include <limitless/ms/material_compiler.hpp>

namespace Limitless {
    class EffectInstance;
}

namespace Limitless::fx {
    class AbstractEmitter;
    class SpriteEmitter;
    class MeshEmitter;

    class EffectCompiler : public ms::MaterialCompiler {
    private:
        static std::string getEmitterDefines(const AbstractEmitter& emitter) noexcept;

        template<typename T>
        void compile(ShaderType shader_type, const T& emitter);
    public:
        explicit EffectCompiler(Context& context, Assets& assets, const RenderSettings& settings);

        using ShaderCompiler::compile;
        void compile(const EffectInstance& instance, ShaderType material_shader);
    };
}