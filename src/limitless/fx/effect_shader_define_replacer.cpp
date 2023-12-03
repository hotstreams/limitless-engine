#include <limitless/fx/effect_shader_define_replacer.hpp>

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless::fx;

std::string EffectShaderDefineReplacer::getEmitterDefines(const AbstractEmitter& emitter) noexcept {
    std::string defines;

    switch (emitter.getType()) {
        case AbstractEmitter::Type::Sprite:
            defines.append("#define SpriteEmitter\n");
            break;
        case AbstractEmitter::Type::Mesh:
            defines.append("#define MeshEmitter\n");
            break;
        case AbstractEmitter::Type::Beam:
            defines.append("#define BeamEmitter\n");
            break;
    }

    for (const auto& type : emitter.getUniqueShaderType().module_type) {
        switch (type) {
            case fx::ModuleType::InitialLocation:
                defines.append("#define InitialLocation_MODULE\n");
                break;
            case fx::ModuleType ::InitialRotation:
                defines.append("#define InitialRotation_MODULE\n");
                break;
            case fx::ModuleType ::InitialVelocity:
                defines.append("#define InitialVelocity_MODULE\n");
                break;
            case fx::ModuleType ::InitialColor:
                defines.append("#define InitialColor_MODULE\n");
                break;
            case fx::ModuleType ::InitialSize:
                defines.append("#define InitialSize_MODULE\n");
                break;
            case fx::ModuleType ::InitialAcceleration:
                defines.append("#define InitialAcceleration_MODULE\n");
                break;
            case fx::ModuleType ::InitialMeshLocation:
                defines.append("#define InitialMeshLocation_MODULE\n");
                break;
            case fx::ModuleType ::MeshLocationAttachment:
                defines.append("#define MeshLocationAttachment_MODULE\n");
                break;
            case fx::ModuleType ::Lifetime:
                defines.append("#define Lifetime_MODULE\n");
                break;
            case fx::ModuleType ::SubUV:
                defines.append("#define SubUV_MODULE\n");
                break;
            case fx::ModuleType ::ColorByLife:
                defines.append("#define ColorByLife_MODULE\n");
                break;
            case fx::ModuleType ::RotationRate:
                defines.append("#define RotationRate_MODULE\n");
                break;
            case fx::ModuleType ::SizeByLife:
                defines.append("#define SizeByLife_MODULE\n");
                break;
            case fx::ModuleType::CustomMaterial:
                defines.append("#define CustomMaterial_MODULE\n");
                break;
            case fx::ModuleType::CustomMaterialByLife:
                defines.append("#define CustomMaterialByLife_MODULE\n");
                break;
            case fx::ModuleType::VelocityByLife:
                defines.append("#define VelocityByLife_MODULE\n");
                break;
            case ModuleType::Beam_InitialDisplacement:
                defines.append("#define Beam_InitialDisplacement_MODULE\n");
                break;
            case ModuleType::Beam_InitialOffset:
                defines.append("#define Beam_InitialOffset_MODULE\n");
                break;
            case ModuleType::Beam_InitialRebuild:
                defines.append("#define Beam_InitialRebuild_MODULE\n");
                break;
            case ModuleType::Beam_InitialTarget:
                defines.append("#define Beam_InitialTarget_MODULE\n");
                break;
            case ModuleType::BeamBuilder:
                defines.append("#define BeamBuilder_MODULE\n");
                break;
            case ModuleType::BeamSpeed:
                defines.append("#define BeamSpeed_MODULE\n");
                break;
            case ModuleType::Time:
                defines.append("#define Time_MODULE\n");
                break;
        }
    }
    return defines;
}

void EffectShaderDefineReplacer::replaceMaterialDependentDefine(Shader& shader, const ms::Material& material, InstanceType model_shader, const AbstractEmitter& emitter) {
    shader.replaceKey(DEFINE_NAMES.at(Define::MaterialDependent), getMaterialDependentDefine(material, model_shader) + getEmitterDefines(emitter));

    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Vertex], material.getVertexSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Fragment], material.getFragmentSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Global], material.getGlobalSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomScalar], getScalarUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomSamplers], getSamplerUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomShading], material.getShadingSnippet());
}
