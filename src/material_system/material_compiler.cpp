#include <material_system/material_compiler.hpp>

#include <render_settings.hpp>
#include <shader_storage.hpp>
#include <material_system/material.hpp>
#include <material_system/custom_material_builder.hpp>

using namespace GraphicsEngine;

std::string MaterialCompiler::getMaterialDefines(const Material& material) noexcept {
    std::string property_defines;
    for (const auto& [property, ptr] : material.properties) {
        switch (property) {
            case PropertyType::Color:
                property_defines.append("#define MATERIAL_COLOR\n");
                break;
            case PropertyType::EmissiveColor:
                property_defines.append("#define MATERIAL_EMISSIVE_COLOR\n");
                break;
            case PropertyType::Diffuse:
                property_defines.append("#define MATERIAL_DIFFUSE\n");
                break;
            case PropertyType::Specular:
                property_defines.append("#define MATERIAL_SPECULAR\n");
                break;
            case PropertyType::Normal:
                property_defines.append("#define MATERIAL_NORMAL\n");
                break;
            case PropertyType::Displacement:
                property_defines.append("#define MATERIAL_DISPLACEMENT\n");
                break;
            case PropertyType::EmissiveMask:
                property_defines.append("#define MATERIAL_EMISSIVEMASK\n");
                break;
            case PropertyType::BlendMask:
                property_defines.append("#define MATERIAL_BLENDMASK\n");
                break;
            case PropertyType::Shininess:
                property_defines.append("#define MATERIAL_SHININESS\n");
                break;
            case PropertyType::Metallic:
                property_defines.append("#define MATERIAL_METALLIC\n");
                break;
            case PropertyType::Roughness:
                property_defines.append("#define MATERIAL_ROUGHNESS\n");
                break;
            case PropertyType::MetallicTexture:
                property_defines.append("#define MATERIAL_METALLIC_TEXTURE\n");
                break;
            case PropertyType::RoughnessTexture:
                property_defines.append("#define MATERIAL_ROUGHNESS_TEXTURE\n");
                break;
        }
    }

    switch (material.shading) {
        case Shading::Lit:
            property_defines.append("#define MATERIAL_LIT\n");
            break;
        case Shading::Unlit:
            property_defines.append("#define MATERIAL_UNLIT\n");
            break;
    }

    if (material.isCustom()) {
        property_defines.append("#define CUSTOM_MATERIAL\n");
    } else {
        property_defines.append("#define REGULAR_MATERIAL\n");
    }

    auto prop_exist = [&] (PropertyType exist) {
        return std::find_if(material.properties.begin(), material.properties.end(), [&] (const auto& property) { return property.first == exist; }) != material.properties.end();
    };

    if ((prop_exist(PropertyType::MetallicTexture) || prop_exist(PropertyType::RoughnessTexture) || prop_exist(PropertyType::Metallic) || prop_exist(PropertyType::Roughness)) &&
        render_settings.physically_based_rendering) {
        property_defines.append("#define PBR\n");
    }

    return property_defines;
}

std::string MaterialCompiler::getModelDefines(const ModelShader& type) noexcept {
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

std::string MaterialCompiler::getCustomMaterialScalarUniforms(const CustomMaterial& material) noexcept {
    std::string uniforms;
    for (const auto& [name, uniform] : material.uniforms) {
        if (uniform->getType() == UniformType::Value) {
            uniforms.append(getUniformDeclaration(*uniform));
        }
    }
    return uniforms;
}

std::string MaterialCompiler::getCustomMaterialSamplerUniforms(const CustomMaterial& material) noexcept {
    std::string uniforms;
    for (const auto& [name, uniform] : material.uniforms) {
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

    shader.replaceKey("GraphicsEngine::Settings", settings);
}

void MaterialCompiler::replaceMaterialSettings(Shader& shader, const Material& material, ModelShader model_shader) noexcept {
    shader.replaceKey("GraphicsEngine::MaterialType", getMaterialDefines(material));
    shader.replaceKey("GraphicsEngine::ModelType", getModelDefines(model_shader));

    if (material.isCustom()) {
        const auto& custom = static_cast<const CustomMaterial&>(material);

        shader.replaceKey("GraphicsEngine::CustomMaterialVertexCode", custom.getVertexCode());
        shader.replaceKey("GraphicsEngine::CustomMaterialFragmentCode", custom.getFragmentCode());
        shader.replaceKey("GraphicsEngine::CustomMaterialScalarUniforms", getCustomMaterialScalarUniforms(custom));
        shader.replaceKey("GraphicsEngine::CustomMaterialSamplerUniforms", getCustomMaterialSamplerUniforms(custom));
    }
}

void MaterialCompiler::compile(const Material& material, MaterialShader material_shader, ModelShader model_shader) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, material, model_shader);
        replaceRenderSettings(shader);
    };

    shader_storage.add(material_shader, model_shader, material.shader_index, compile(SHADER_DIR + material_shader_path.at(material_shader), props));
}
