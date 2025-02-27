#include <limitless/ms/material_compiler.hpp>

#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/ms/material_shader_define_replacer.hpp>

using namespace Limitless::ms;

MaterialCompiler::MaterialCompiler(Context& context, Assets& _assets, const RendererSettings& settings) noexcept
    : ShaderCompiler {context, settings}
    , assets {_assets} {
}

void MaterialCompiler::replaceMaterialSettings(Shader& shader, const Material& material,const RendererSettings& settings, InstanceType model_shader) noexcept {
    MaterialShaderDefineReplacer::replaceMaterialDependentDefine(shader, material, settings, model_shader);
}

void MaterialCompiler::compile(const Material& material, ShaderType pass_shader, InstanceType model_shader) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, material, render_settings.value(), model_shader);
    };

	try {
		std::shared_ptr<ShaderProgram> shader;
		if (material.getUniforms().count("skybox"))
		{
			shader = compile(assets.getShaderDir() / SHADER_PASS_PATH.at(pass_shader), props);
		} else {
			shader = compile(assets.getShaderDir() / "pipeline/common", assets.getShaderDir() / SHADER_PASS_PATH.at(pass_shader), props);
		}

		assets.shaders.add(pass_shader, model_shader, material.getShaderIndex(), shader);
	} catch (const std::exception& e) {
		throw material_compilation_error {material.getName() + e.what()};
	}
}
