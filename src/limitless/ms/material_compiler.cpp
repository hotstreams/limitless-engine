#include <limitless/ms/material_compiler.hpp>

#include <limitless/renderer/render_settings.hpp>
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
                property_defines.append("#define ENGINE_MATERIAL_COLOR\n");
                break;
            case Property::EmissiveColor:
                property_defines.append("#define ENGINE_MATERIAL_EMISSIVE_COLOR\n");
                break;
            case Property::Diffuse:
                property_defines.append("#define ENGINE_MATERIAL_DIFFUSE_TEXTURE\n");
                break;
            case Property::Normal:
                property_defines.append("#define ENGINE_MATERIAL_NORMAL_TEXTURE\n");
                break;
            case Property::EmissiveMask:
                property_defines.append("#define ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE\n");
                break;
            case Property::BlendMask:
                property_defines.append("#define ENGINE_MATERIAL_BLENDMASK_TEXTURE\n");
                break;
            case Property::Metallic:
                property_defines.append("#define ENGINE_MATERIAL_METALLIC\n");
                break;
            case Property::Roughness:
                property_defines.append("#define ENGINE_MATERIAL_ROUGHNESS\n");
                break;
            case Property::MetallicTexture:
                property_defines.append("#define ENGINE_MATERIAL_METALLIC_TEXTURE\n");
                break;
            case Property::RoughnessTexture:
                property_defines.append("#define ENGINE_MATERIAL_ROUGHNESS_TEXTURE\n");
                break;
            case Property::TessellationFactor:
                property_defines.append("#define ENGINE_MATERIAL_TESSELLATION_FACTOR\n");
                break;
            case Property::AmbientOcclusionTexture:
                property_defines.append("#define ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE\n");
                break;
            case Property::IoR:
                property_defines.append("#define ENGINE_MATERIAL_IOR\n");
                break;
            case Property::Absorption:
                property_defines.append("#define ENGINE_MATERIAL_ABSORPTION\n");
                break;
            case Property::ORM:
                property_defines.append("#define ENGINE_MATERIAL_ORM_TEXTURE\n");
                break;
        }
    }

    if (material.getRefraction()) {
        property_defines.append("#define ENGINE_MATERIAL_REFRACTION\n");
    }

    switch (material.getShading()) {
        case Shading::Lit:
            property_defines.append("#define ENGINE_MATERIAL_SHADING_REGULAR_MODEL\n");
            break;
        case Shading::Unlit:
            property_defines.append("#define ENGINE_MATERIAL_SHADING_UNLIT_MODEL\n");
            break;
        case Shading::Cloth:
            property_defines.append("#define ENGINE_MATERIAL_SHADING_CLOTH_MODEL\n");
            break;
        case Shading::Subsurface:
            property_defines.append("#define ENGINE_MATERIAL_SHADING_SUBSURFACE_MODEL\n");
            break;
        case Shading::Custom:
            property_defines.append("#define ENGINE_MATERIAL_SHADING_CUSTOM_MODEL\n");
            break;
    }

    return property_defines;
}

std::string MaterialCompiler::getModelDefines(const InstanceType& type) {
    std::string defines;
    switch (type) {
        case InstanceType::Model:
            defines.append("#define SIMPLE_MODEL\n");
            break;
        case InstanceType::Skeletal:
            defines.append("#define SKELETAL_MODEL\n");
            break;
        case InstanceType::Instanced:
            defines.append("#define INSTANCED_MODEL\n");
            break;
        case InstanceType::Effect:
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

void MaterialCompiler::replaceMaterialSettings(Shader& shader, const Material& material, InstanceType model_shader) noexcept {
    shader.replaceKey("Limitless::MaterialType", getMaterialDefines(material));
    shader.replaceKey("Limitless::ModelType", getModelDefines(model_shader));
    shader.replaceKey("Limitless::EmitterType", "");

    shader.replaceKey("_MATERIAL_VERTEX_SNIPPET", material.getVertexSnippet());
    shader.replaceKey("_MATERIAL_FRAGMENT_SNIPPET", material.getFragmentSnippet());
    shader.replaceKey("ENGINE_MATERIAL_GLOBAL_DEFINITIONS", material.getGlobalSnippet());
    shader.replaceKey("_MATERIAL_TESSELLATION_SNIPPET", material.getTessellationSnippet());
    shader.replaceKey("ENGINE_MATERIAL_CUSTOM_SCALARS", getCustomMaterialScalarUniforms(material));
    shader.replaceKey("ENGINE_MATERIAL_CUSTOM_SAMPLERS", getCustomMaterialSamplerUniforms(material));
}

void MaterialCompiler::compile(const Material& material, ShaderType pass_shader, InstanceType model_shader) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, material, model_shader);
        replaceRenderSettings(shader);
    };

    if (material.contains(Property::TessellationFactor)) {
        auto tesc = Shader { assets.getShaderDir() / "tesselation" / "tesselation.tesc", Shader::Type::TessControl, props };
        auto tese = Shader { assets.getShaderDir() / "tesselation" / "tesselation.tese", Shader::Type::TessEval, props };

        *this << std::move(tesc)
              << std::move(tese);
    }

	try {
		auto shader = compile(assets.getShaderDir() / SHADER_PASS_PATH.at(pass_shader), props);
		assets.shaders.add(pass_shader, model_shader, material.getShaderIndex(), shader);
	} catch (const std::exception& e) {
		throw material_compilation_error{material.getName() + e.what()};
	}
}
