#pragma once

#include <limitless/ms/material_shader_define_replacer.hpp>

namespace Limitless::ms {
    class Material;
}

namespace Limitless::fx {
    class AbstractEmitter;

    class EffectShaderDefineReplacer : public ms::MaterialShaderDefineReplacer {
    private:
        static std::string getEmitterDefines(const AbstractEmitter& emitter) noexcept;
    public:
        static void replaceMaterialDependentDefine(Shader& shader, const ms::Material& material, InstanceType model_shader, const AbstractEmitter& emitter);
    };
}