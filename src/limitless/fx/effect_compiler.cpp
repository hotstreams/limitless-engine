#include <limitless/fx/effect_compiler.hpp>

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/assets.hpp>

using namespace Limitless::fx;
using namespace Limitless;

EffectCompiler::EffectCompiler(Context& context, Assets& assets, const RenderSettings& settings)
    : MaterialCompiler(context, assets, settings) {
}

std::string EffectCompiler::getEmitterDefines(const AbstractEmitter& emitter) noexcept {
    std::string defines;

    switch (emitter.getType()) {
        case AbstractEmitter::Type::Sprite:
            defines.append("#define SpriteEmitter\n");
            break;
        case AbstractEmitter::Type::Mesh:
            defines.append("#define MeshEmitter\n");
            break;
        case AbstractEmitter::Type::Beam:
            defines.append("#define BeamEmitter\n");
            break;
    }

    for (const auto& type : emitter.getUniqueShaderType().module_type) {
        switch (type) {
            case fx::ModuleType::InitialLocation:
                defines.append("#define InitialLocation_MODULE\n");
                break;
            case fx::ModuleType ::InitialRotation:
                defines.append("#define InitialRotation_MODULE\n");
                break;
            case fx::ModuleType ::InitialVelocity:
                defines.append("#define InitialVelocity_MODULE\n");
                break;
            case fx::ModuleType ::InitialColor:
                defines.append("#define InitialColor_MODULE\n");
                break;
            case fx::ModuleType ::InitialSize:
                defines.append("#define InitialSize_MODULE\n");
                break;
            case fx::ModuleType ::InitialAcceleration:
                defines.append("#define InitialAcceleration_MODULE\n");
                break;
            case fx::ModuleType ::InitialMeshLocation:
                defines.append("#define InitialMeshLocation_MODULE\n");
                break;
            case fx::ModuleType ::MeshLocationAttachment:
                defines.append("#define MeshLocationAttachment_MODULE\n");
                break;
            case fx::ModuleType ::Lifetime:
                defines.append("#define Lifetime_MODULE\n");
                break;
            case fx::ModuleType ::SubUV:
                defines.append("#define SubUV_MODULE\n");
                break;
            case fx::ModuleType ::ColorByLife:
                defines.append("#define ColorByLife_MODULE\n");
                break;
            case fx::ModuleType ::RotationRate:
                defines.append("#define RotationRate_MODULE\n");
                break;
            case fx::ModuleType ::SizeByLife:
                defines.append("#define SizeByLife_MODULE\n");
                break;
            case fx::ModuleType::CustomMaterial:
                defines.append("#define CustomMaterial_MODULE\n");
                break;
            case fx::ModuleType::CustomMaterialByLife:
                defines.append("#define CustomMaterialByLife_MODULE\n");
                break;
            case fx::ModuleType::VelocityByLife:
                defines.append("#define VelocityByLife_MODULE\n");
                break;
            case ModuleType::Beam_InitialDisplacement:
                defines.append("#define Beam_InitialDisplacement_MODULE\n");
                break;
            case ModuleType::Beam_InitialOffset:
                defines.append("#define Beam_InitialOffset_MODULE\n");
                break;
            case ModuleType::Beam_InitialRebuild:
                defines.append("#define Beam_InitialRebuild_MODULE\n");
                break;
            case ModuleType::Beam_InitialTarget:
                defines.append("#define Beam_InitialTarget_MODULE\n");
                break;
            case ModuleType::BeamBuilder:
                defines.append("#define BeamBuilder_MODULE\n");
                break;
            case ModuleType::BeamSpeed:
                defines.append("#define BeamSpeed_MODULE\n");
                break;
            case ModuleType::Time:
                defines.append("#define Time_MODULE\n");
                break;
        }
    }
    return defines;
}

template<typename T>
void EffectCompiler::compile(ShaderType shader_type, const T& emitter) {
    if (!assets.shaders.reserveIfNotContains({emitter.getUniqueShaderType(), shader_type})) {
        const auto props = [&] (Shader& shader) {
            shader.replaceKey("Limitless::EmitterType", getEmitterDefines(emitter));

            replaceMaterialSettings(shader, emitter.getMaterial(), InstanceType::Effect);
            replaceRenderSettings(shader);
        };

        assets.shaders.add({emitter.getUniqueShaderType(), shader_type}, compile(assets.getShaderDir() / SHADER_PASS_PATH.at(shader_type), props));
    }
}

void EffectCompiler::compile(const EffectInstance& instance, ShaderType shader_type) {
	try {
		for (const auto& [name, emitter] : instance.getEmitters()) {
			switch (emitter->getType()) {
				case fx::AbstractEmitter::Type::Sprite:
					compile(shader_type, instance.get<fx::SpriteEmitter>(name));
					break;
				case fx::AbstractEmitter::Type::Mesh:
					compile(shader_type, instance.get<fx::MeshEmitter>(name));
					break;
				case fx::AbstractEmitter::Type::Beam:
					compile(shader_type, instance.get<fx::BeamEmitter>(name));
					break;
			}
		}
	} catch (const std::exception& e) {
		throw std::runtime_error{instance.getName() + e.what()};
	}
}
