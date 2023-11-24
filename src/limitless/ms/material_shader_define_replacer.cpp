#include <limitless/ms/material_shader_define_replacer.hpp>

#include <limitless/ms/material.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/core/shader/shader_define_replacer.hpp>
#include <limitless/core/uniform/uniform.hpp>

using namespace Limitless::ms;

std::string MaterialShaderDefineReplacer::getPropertyDefines(const Material& material) {
    std::string defines;

    for (const auto& [property, _] : material.getProperties()) {
        auto define = "#define " + PROPERTY_DEFINE.at(property) + '\n';
        defines.append(define);
    }

    if (material.getRefraction()) {
        defines.append("#define ENGINE_MATERIAL_REFRACTION\n");
    }

    if (material.getTwoSided()) {
        defines.append("#define ENGINE_MATERIAL_TWO_SIDED\n");
    }

    return defines;
}

std::string MaterialShaderDefineReplacer::getShadingDefines(const Material& material) {
    return "#define " + SHADING_DEFINE[material.getShading()] + '\n';
}

std::string MaterialShaderDefineReplacer::getModelDefines(InstanceType model_shader) {
    return "#define " + MODEL_DEFINE[model_shader] + '\n';
}

void MaterialShaderDefineReplacer::replaceMaterialDependentDefine(Shader &shader, const Material &material, InstanceType model_shader) {
    shader.replaceKey(DEFINE_NAMES.at(Define::MaterialDependent), getMaterialDependentDefine(material, model_shader));

    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Vertex], material.getVertexSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Fragment], material.getFragmentSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Global], material.getGlobalSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomScalar], getScalarUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomSamplers], getSamplerUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomShading], material.getShadingSnippet());
}

std::string MaterialShaderDefineReplacer::getScalarUniformDefines(const Material &material) {
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

std::string MaterialShaderDefineReplacer::getSamplerUniformDefines(const Material &material) {
    std::string uniforms;
    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() == UniformType::Sampler) {
            uniforms.append(getUniformDeclaration(*uniform));
        }
    }
    return uniforms;
}

std::string MaterialShaderDefineReplacer::getMaterialDefines(const Material &material) {
    std::string define = getPropertyDefines(material);
    define.append(getShadingDefines(material));
    return define;
}

std::string MaterialShaderDefineReplacer::getMaterialDependentDefine(const Material &material, InstanceType model_shader) {
    std::string define = getMaterialDefines(material);
    define.append(getModelDefines(model_shader));
    return define;
}
