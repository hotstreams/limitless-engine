#include <material_compiler.hpp>

#include <render_settings.hpp>
#include <shader_storage.hpp>
#include <material.hpp>
#include <custom_material_builder.hpp>

using namespace GraphicsEngine;

void MaterialCompiler::replaceSettings(Shader& shader) noexcept {
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

std::string MaterialCompiler::getMaterialDefines(const MaterialType& type) noexcept {
    std::string property_defines;
    for (const auto& property : type.properties) {
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

    switch (type.shading) {
        case Shading::Lit:
            property_defines.append("#define MATERIAL_LIT\n");
            break;
        case Shading::Unlit:
            property_defines.append("#define MATERIAL_UNLIT\n");
            break;
    }

    auto prop_exist = [&] (PropertyType exist) {
        auto found = std::find_if(type.properties.begin(), type.properties.end(), [&] (const auto& property) { return property == exist; });
        return found != type.properties.end();
    };

    if ((prop_exist(PropertyType::Metallic) || prop_exist(PropertyType::Roughness) ||  prop_exist(PropertyType::MetallicTexture) || prop_exist(PropertyType::RoughnessTexture)) && render_settings.physically_based_rendering) {
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
    }

    return defines;
}

std::string MaterialCompiler::getCustomMaterialScalarUniforms(const CustomMaterialBuilder& builder) noexcept {
    std::string uniforms;
    for (const auto& [name, uniform] : builder.getUniforms()) {
        if (uniform->getType() == UniformType::Value) {
            uniforms.append(getUniformDeclaration(*uniform));
        }
    }
    return uniforms;
}

std::string MaterialCompiler::getCustomMaterialSamplerUniforms(const CustomMaterialBuilder& builder) noexcept {
    std::string uniforms;
    for (const auto& [name, uniform] : builder.getUniforms()) {
        if (uniform->getType() == UniformType::Sampler) {
            uniforms.append(getUniformDeclaration(*uniform));
        }
    }
    return uniforms;
}

void MaterialCompiler::compile(const MaterialBuilder& builder, MaterialShader material_type, ModelShader model_type) {
    auto material_defines = getMaterialDefines(builder.getMaterialType());
    material_defines.append("#define REGULAR_MATERIAL\n");

    const auto props = [&] (Shader& shader) {
        shader.replaceKey("GraphicsEngine::MaterialType", material_defines);
        shader.replaceKey("GraphicsEngine::ModelType", getModelDefines(model_type));
        replaceSettings(shader);
    };

    shader_storage.add(material_type, model_type, builder.getMaterialIndex(), compile(SHADER_DIR + material_shader_path.at(material_type), props));
}

void MaterialCompiler::compile(const CustomMaterialBuilder& builder, MaterialShader material_type, ModelShader model_type) {
    auto material_defines = getMaterialDefines(builder.getMaterialType());
    material_defines.append("#define CUSTOM_MATERIAL\n");

    const auto props = [&] (Shader& shader) {
        shader.replaceKey("GraphicsEngine::MaterialType", material_defines);
        shader.replaceKey("GraphicsEngine::ModelType", getModelDefines(model_type));
        replaceSettings(shader);

        shader.replaceKey("GraphicsEngine::CustomMaterialVertexCode", builder.getVertexCode());
        shader.replaceKey("GraphicsEngine::CustomMaterialFragmentCode", builder.getFragmentCode());
        shader.replaceKey("GraphicsEngine::CustomMaterialScalarUniforms", getCustomMaterialScalarUniforms(builder));
        shader.replaceKey("GraphicsEngine::CustomMaterialSamplerUniforms", getCustomMaterialSamplerUniforms(builder));
    };

    shader_storage.add(material_type, model_type, builder.getMaterialIndex(), compile(SHADER_DIR + material_shader_path.at(material_type), props));
}

void MaterialCompiler::compile(const SpriteEmitter& emitter) {
    MaterialBuilder builder;
    auto material_defines = getMaterialDefines(builder.getMaterialType(emitter.getMaterial()));
    material_defines.append(emitter.getMaterial()->isCustom() ? "#define CUSTOM_MATERIAL\n" : "#define REGULAR_MATERIAL\n");

    const auto props = [&] (Shader& shader) {
        shader.replaceKey("GraphicsEngine::MaterialType", material_defines);
        shader.replaceKey("GraphicsEngine::EmitterType", getEmitterDefines(emitter));
        replaceSettings(shader);

//        if (emitter.getMaterial()->isCustom()) {
//            replaceKey(src, "GraphicsEngine::CustomMaterialVertexCode", builder.getVertexCode());
//            replaceKey(src, "GraphicsEngine::CustomMaterialFragmentCode", builder.getFragmentCode());
//            replaceKey(src, "GraphicsEngine::CustomMaterialScalarUniforms", getCustomMaterialScalarUniforms(builder));
//            replaceKey(src, "GraphicsEngine::CustomMaterialSamplerUniforms", getCustomMaterialSamplerUniforms(builder));
//        }
    };

    shader_storage.add(emitter.getEmitterType(), compile(SHADER_DIR "effects/sprite_emitter", props));
}

std::string MaterialCompiler::getEmitterDefines(const SpriteEmitter& emitter) noexcept {
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
//            case EmitterModuleType ::AccelerationByLife:
//                defines.append("#define AccelerationByLife_MODULE\n");
//                break;
            case EmitterModuleType ::ColorByLife:
                defines.append("#define ColorByLife_MODULE\n");
                break;
            case EmitterModuleType ::RotationRate:
                defines.append("#define RotationRate_MODULE\n");
                break;
            case EmitterModuleType ::SizeByLife:
                defines.append("#define SizeByLife_MODULE\n");
                break;
//            case EmitterModuleType::SphereLocation:
//                defines.append("#define SphereLocation_MODULE\n");
//                break;
//            case EmitterModuleType::CustomMaterial:
//                defines.append("#define CustomMaterial_MODULE\n");
//                break;
        }
    }
    return defines;
}