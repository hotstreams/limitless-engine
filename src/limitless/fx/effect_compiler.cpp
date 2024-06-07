#include <limitless/fx/effect_compiler.hpp>

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/assets.hpp>
#include <limitless/fx/effect_shader_define_replacer.hpp>

using namespace Limitless::fx;
using namespace Limitless;

EffectCompiler::EffectCompiler(Context& context, Assets& assets, const RendererSettings& settings)
    : MaterialCompiler(context, assets, settings) {
}

template<typename T>
void EffectCompiler::compile(ShaderType shader_type, const T& emitter) {
    if (!assets.shaders.reserveIfNotContains({emitter.getUniqueShaderType(), shader_type})) {
        const auto props = [&] (Shader& shader) {
            EffectShaderDefineReplacer::replaceMaterialDependentDefine(shader, emitter.getMaterial(), InstanceType::Effect, emitter);
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
