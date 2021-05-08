#include <limitless/ms/material_compiler.hpp>

#include <limitless/pipeline/render_settings.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>

using namespace Limitless::ms;

MaterialCompiler::MaterialCompiler(Context& context, Assets& _assets, const RenderSettings& _settings) noexcept
    :  ShaderCompiler {context}
    , assets {_assets}
    , render_settings {_settings} {
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
            case Property::Specular:
                property_defines.append("#define MATERIAL_SPECULAR\n");
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

    if ((material.contains(Property::MetallicTexture) || material.contains(Property::RoughnessTexture) ||
         material.contains(Property::Metallic) || material.contains(Property::Roughness)) &&
        render_settings.physically_based_render) {
        property_defines.append("#define PBR\n");
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
        case ModelShader::SkeletalInstanced:
            defines.append("#define SKELETAL_INSTANCED_MODEL\n");
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

void MaterialCompiler::replaceRenderSettings(Shader& shader) noexcept {
    std::string settings;

    // sets shading model
    switch (render_settings.shading_model) {
        case ShadingModel::Phong:
            settings.append("#define PHONG_MODEL\n");
            break;
        case ShadingModel::BlinnPhong:
            settings.append("#define BLINN_PHONG_MODEL\n");
            break;
    }

    // sets normal mapping
    if (render_settings.normal_mapping) {
        settings.append("#define NORMAL_MAPPING\n");
    }

    if (render_settings.directional_csm) {
        settings.append("#define DIRECTIONAL_CSM\n");

        settings.append("#define DIRECTIONAL_SPLIT_COUNT " + std::to_string(render_settings.directional_split_count) + '\n');

        if (render_settings.directional_pcf) {
            settings.append("#define DIRECTIONAL_PFC\n");
        }
    }

    shader.replaceKey("Limitless::Settings", settings);
}

void MaterialCompiler::replaceMaterialSettings(Shader& shader, const Material& material, ModelShader model_shader) noexcept {
    shader.replaceKey("Limitless::MaterialType", getMaterialDefines(material));
    shader.replaceKey("Limitless::ModelType", getModelDefines(model_shader));

    shader.replaceKey("Limitless::CustomMaterialVertexCode", material.getVertexSnippet());
    shader.replaceKey("Limitless::CustomMaterialFragmentCode", material.getFragmentSnippet());
    shader.replaceKey("Limitless::CustomMaterialGlobalDefinitions", material.getGlobalSnippet());
    shader.replaceKey("Limitless::CustomMaterialTessellationCode", material.getTessellationSnippet());
    shader.replaceKey("Limitless::CustomMaterialScalarUniforms", getCustomMaterialScalarUniforms(material));
    shader.replaceKey("Limitless::CustomMaterialSamplerUniforms", getCustomMaterialSamplerUniforms(material));
}

void MaterialCompiler::compile(const Material& material, ShaderPass pass_shader, ModelShader model_shader) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, material, model_shader);
        replaceRenderSettings(shader);
    };

    if (material.contains(Property::TessellationFactor)) {
        *this << Shader { SHADER_DIR "tesselation" PATH_SEPARATOR "tesselation.tcs", Shader::Type::TessControl, props }
              << Shader { SHADER_DIR "tesselation" PATH_SEPARATOR "tesselation.tes", Shader::Type::TessEval, props };
    }

    assets.shaders.add(pass_shader, model_shader, material.getShaderIndex(), compile(SHADER_DIR + SHADER_PASS_PATH.at(pass_shader), props));
}
