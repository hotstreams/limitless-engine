#include <limitless/ms/material_shader_definer.hpp>

#include <limitless/ms/material.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/core/shader/shader_defines.hpp>

using namespace Limitless::ms;

std::string MaterialShaderDefiner::getPropertyDefines(const Material& material) {
    std::string defines;

    for (const auto& [property, _] : material.getProperties()) {
        defines.append(SHADER_PROPERTY_DEFINE[property]);
    }

    if (material.getRefraction()) {
        defines.append("#define ENGINE_MATERIAL_REFRACTION\n");
    }
}

std::string MaterialShaderDefiner::getShadingDefines(const Material& material) {
    return SHADER_SHADING_DEFINE[material.getShading()];
}

std::string MaterialShaderDefiner::getModelDefines(InstanceType model_shader) {
    return SHADER_MODEL_DEFINE[model_shader];
}

void MaterialShaderDefiner::define(Shader &shader, const Material &material,  InstanceType model_shader) {
    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::MaterialType], getMaterialDefines(material));
    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::ModelType], getModelDefines(model_shader));
    shader.replaceKey("Limitless::EmitterType", "");

    shader.replaceKey(SHADER_SNIPPET_DEFINE[SnippetDefineType::Vertex], material.getVertexSnippet());
    shader.replaceKey(SHADER_SNIPPET_DEFINE[SnippetDefineType::Tesselation], material.getTessellationSnippet());
    shader.replaceKey(SHADER_SNIPPET_DEFINE[SnippetDefineType::Fragment], material.getFragmentSnippet());
    shader.replaceKey(SHADER_SNIPPET_DEFINE[SnippetDefineType::Global], material.getGlobalSnippet());
    shader.replaceKey(SHADER_SNIPPET_DEFINE[SnippetDefineType::CustomScalar], getScalarUniformDefines(material));
    shader.replaceKey(SHADER_SNIPPET_DEFINE[SnippetDefineType::CustomSamplers], getSamplerUniformDefines(material));
}

std::string MaterialShaderDefiner::getScalarUniformDefines(const Material &material) {
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

std::string MaterialShaderDefiner::getSamplerUniformDefines(const Material &material) {
    std::string uniforms;
    for (const auto& [name, uniform] : material.getUniforms()) {
        if (uniform->getType() == UniformType::Sampler) {
            uniforms.append(getUniformDeclaration(*uniform));
        }
    }
    return uniforms;
}

std::string MaterialShaderDefiner::getMaterialDefines(const Material &material) {
    std::string defines = getPropertyDefines(material);
    defines.append(getShadingDefines(material));
    return defines;
}
