#include <limitless/ms/material_compiler.hpp>

#include <limitless/renderer/render_settings.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/ms/material_shader_definer.hpp>

using namespace Limitless::ms;

MaterialCompiler::MaterialCompiler(Context& context, Assets& _assets, const RenderSettings& settings) noexcept
    : ShaderCompiler {context, settings}
    , assets {_assets} {
}

void MaterialCompiler::replaceMaterialSettings(Shader& shader, const Material& material, InstanceType model_shader) noexcept {
    MaterialShaderDefiner::define(shader, material, model_shader);
}

void MaterialCompiler::compile(const Material& material, ShaderType pass_shader, InstanceType model_shader) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, material, model_shader);
        replaceEngineDefines(shader);
    };

//    if (material.contains(Property::TessellationFactor)) {
//        auto tesc = Shader { assets.getShaderDir() / "tesselation" / "tesselation.tesc", Shader::Type::TessControl, props };
//        auto tese = Shader { assets.getShaderDir() / "tesselation" / "tesselation.tese", Shader::Type::TessEval, props };
//
//        *this << std::move(tesc)
//              << std::move(tese);
//    }

	try {
		auto shader = compile(assets.getShaderDir() / SHADER_PASS_PATH.at(pass_shader), props);
		assets.shaders.add(pass_shader, model_shader, material.getShaderIndex(), shader);
	} catch (const std::exception& e) {
		throw material_compilation_error {material.getName() + e.what()};
	}
}
