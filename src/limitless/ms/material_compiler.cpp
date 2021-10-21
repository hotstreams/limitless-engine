#include <limitless/ms/material_compiler.hpp>

#include <limitless/pipeline/render_settings.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>

using namespace Limitless::ms;

MaterialCompiler::MaterialCompiler(Context& context, Assets& _assets, const RenderSettings& settings) noexcept
    : ShaderCompiler {context, settings}
    , assets {_assets} {
}

std::string MaterialCompiler::getMaterialDefines(const Material& material) noexcept {
    std::string property_defines;
    for (const auto& [property, ptr] : material.getProperties()) {
        switch (property) {
            case Property::Color:
                property_defines.append("#define MATERIAL_COLOR\n");
                break;
            case Property::EmissiveColor:
                property_defines.append("#define MATERIAL_EMISSIVE_COLOR\n");
                break;
            case Property::Diffuse:
                property_defines.append("#define MATERIAL_DIFFUSE\n");
                break;
            case Property::Normal:
                property_defines.append("#define MATERIAL_NORMAL\n");
                break;
            case Property::EmissiveMask:
                property_defines.append("#define MATERIAL_EMISSIVEMASK\n");
                break;
            case Property::BlendMask:
                property_defines.append("#define MATERIAL_BLENDMASK\n");
                break;
            case Property::Shininess:
                property_defines.append("#define MATERIAL_SHININESS\n");
                break;
            case Property::Metallic:
                property_defines.append("#define MATERIAL_METALLIC\n");
                break;
            case Property::Roughness:
                property_defines.append("#define MATERIAL_ROUGHNESS\n");
                break;
            case Property::MetallicTexture:
                property_defines.append("#define MATERIAL_METALLIC_TEXTURE\n");
                break;
            case Property::RoughnessTexture:
                property_defines.append("#define MATERIAL_ROUGHNESS_TEXTURE\n");
                break;
            case Property::TessellationFactor:
                property_defines.append("#define MATERIAL_TESSELLATION_FACTOR\n");
                break;
            case Property::Refraction:
                property_defines.append("#define MATERIAL_REFRACTION\n");
                break;
            case Property::AmbientOcclusionTexture:
                property_defines.append("#define MATERIAL_AMBIENT_OCCLUSION_TEXTURE\n");
                break;
        }
    }

    switch (material.getShading()) {
        case Shading::Lit:
            property_defines.append("#define MATERIAL_LIT\n");
            break;
        case Shading::Unlit:
            property_defines.append("#define MATERIAL_UNLIT\n");
            break;
    }

    return property_defines;
}

std::string MaterialCompiler::getModelDefines(const ModelShader& type) {
    std::string defines;
    switch (type) {
        case ModelShader::Model:
            defines.append("#define SIMPLE_MODEL\n");
            break;
        case ModelShader::Skeletal:
            defines.append("#define SKELETAL_MODEL\n");
            break;
        case ModelShader::Instanced:
            defines.append("#define INSTANCED_MODEL\n");
            break;
        case ModelShader::Effect:
            defines.append("#define EFFECT_MODEL\n");
            break;
    }
    return defines;
}

std::string MaterialCompiler::getCustomMaterialScalarUniforms(const Material& material) noexcept {
    std::string uniforms;
    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() == UniformType::Value || uniform->getType() == UniformType::Time) {
        	auto decl = getUniformDeclaration(*uniform);
        	decl.erase(decl.find("uniform"), 7);
            uniforms.append(decl);
        }
    }
    return uniforms;
}

std::string MaterialCompiler::getCustomMaterialSamplerUniforms(const Material& material) noexcept {
    std::string uniforms;
    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() == UniformType::Sampler) {
            uniforms.append(getUniformDeclaration(*uniform));
        }
    }
    return uniforms;
}

void MaterialCompiler::replaceMaterialSettings(Shader& shader, const Material& material, ModelShader model_shader) noexcept {
    shader.replaceKey("Limitless::MaterialType", getMaterialDefines(material));
    shader.replaceKey("Limitless::ModelType", getModelDefines(model_shader));
    shader.replaceKey("Limitless::EmitterType", "");

    shader.replaceKey("_MATERIAL_VERTEX_SNIPPET", material.getVertexSnippet());
    shader.replaceKey("_MATERIAL_FRAGMENT_SNIPPET", material.getFragmentSnippet());
    shader.replaceKey("_MATERIAL_GLOBAL_DEFINITIONS", material.getGlobalSnippet());
    shader.replaceKey("_MATERIAL_TESSELLATION_SNIPPET", material.getTessellationSnippet());
    shader.replaceKey("_MATERIAL_SCALAR_UNIFORMS", getCustomMaterialScalarUniforms(material));
    shader.replaceKey("_MATERIAL_SAMPLER_UNIFORMS", getCustomMaterialSamplerUniforms(material));
}

void MaterialCompiler::compile(const Material& material, ShaderPass pass_shader, ModelShader model_shader) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, material, model_shader);
        replaceRenderSettings(shader);
    };

    //TODO: restore
//    if (material.contains(Property::TessellationFactor)) {
//        *this << Shader { assets.getShaderDir() / "tesselation" / "tesselation.tcs", Shader::Type::TessControl, props }
//              << Shader { assets.getShaderDir() / "tesselation" / "tesselation.tes", Shader::Type::TessEval, props };
//    }

	try {
		auto shader = compile(assets.getShaderDir() / SHADER_PASS_PATH.at(pass_shader), props);
		assets.shaders.add(pass_shader, model_shader, material.getShaderIndex(), shader);
	} catch (const std::exception& e) {
		throw material_compilation_error{material.getName() + e.what()};
	}
}
