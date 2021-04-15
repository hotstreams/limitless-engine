#include <limitless/particle_system/effect_compiler.hpp>

#include <limitless/particle_system/mesh_emitter.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

inline const std::map<std::pair<MaterialShader, EmitterType>, std::string> emitter_shader_path = {
    {{MaterialShader::Forward, EmitterType::Sprite},            "effects/sprite_emitter" },
    {{MaterialShader::Forward, EmitterType::Mesh},              "effects/mesh_emitter" },

    {{MaterialShader::DirectionalShadow, EmitterType::Sprite},  "effects/sprite_emitter_csm" },
    {{MaterialShader::DirectionalShadow, EmitterType::Mesh},    "effects/mesh_emitter_csm" },
};

std::string EffectCompiler::getEmitterDefines(const Emitter& emitter) noexcept {
    std::string defines;
    for (const auto& [type, module] : emitter.getModules()) {
        switch (type) {
            case EmitterModuleType::InitialLocation:
                defines.append("#define InitialLocation_MODULE\n");
                break;
            case EmitterModuleType ::InitialRotation:
                defines.append("#define InitialRotation_MODULE\n");
                break;
            case EmitterModuleType ::InitialVelocity:
                defines.append("#define InitialVelocity_MODULE\n");
                break;
            case EmitterModuleType ::InitialColor:
                defines.append("#define InitialColor_MODULE\n");
                break;
            case EmitterModuleType ::InitialSize:
                defines.append("#define InitialSize_MODULE\n");
                break;
            case EmitterModuleType ::InitialAcceleration:
                defines.append("#define InitialAcceleration_MODULE\n");
                break;
            case EmitterModuleType ::MeshLocation:
                defines.append("#define MeshLocation_MODULE\n");
                break;
            case EmitterModuleType ::Lifetime:
                defines.append("#define Lifetime_MODULE\n");
                break;
            case EmitterModuleType ::SubUV:
                defines.append("#define SubUV_MODULE\n");
                break;
            case EmitterModuleType ::ColorByLife:
                defines.append("#define ColorByLife_MODULE\n");
                break;
            case EmitterModuleType ::RotationRate:
                defines.append("#define RotationRate_MODULE\n");
                break;
            case EmitterModuleType ::SizeByLife:
                defines.append("#define SizeByLife_MODULE\n");
                break;
            case EmitterModuleType::CustomMaterial:
                defines.append("#define CustomMaterial_MODULE\n");
                break;
            case EmitterModuleType::CustomMaterialByLife:
                defines.append("#define CustomMaterialByLife_MODULE\n");
                break;
            case EmitterModuleType::VelocityByLife:
                defines.append("#define VelocityByLife_MODULE\n");
                break;
        }
    }
    return defines;
}

template<typename T>
void EffectCompiler::compile(MaterialShader shader_type, const T& emitter) {
    if (!assets.shaders.contains(shader_type, emitter.getEmitterType())) {
        const auto props = [&] (Shader& shader) {
            replaceMaterialSettings(shader, emitter.getMaterial(), ModelShader::Effect);
            replaceRenderSettings(shader);

            shader.replaceKey("LimitlessEngine::EmitterType", getEmitterDefines(emitter));
        };

        assets.shaders.add(shader_type, emitter.getEmitterType(), compile(SHADER_DIR + emitter_shader_path.at({shader_type, emitter.getType()}), props));
    }
}

void EffectCompiler::compile(const EffectInstance& instance, MaterialShader shader_type) {
    for (const auto& [name, emitter] : instance) {
        switch (emitter->getType()) {
            case EmitterType::Sprite:
                compile(shader_type, instance.get<SpriteEmitter>(name));
                break;
            case EmitterType::Mesh:
                compile(shader_type, instance.get<MeshEmitter>(name));
                break;
        }
    }
}
