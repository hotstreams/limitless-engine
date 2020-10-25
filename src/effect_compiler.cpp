#include <effect_compiler.hpp>
#include <mesh_emitter.hpp>
#include <shader_storage.hpp>
#include <effect_instance.hpp>

using namespace GraphicsEngine;

std::string EffectCompiler::getEmitterDefines(const Emitter& emitter) noexcept {
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

void EffectCompiler::compile(const SpriteEmitter& emitter) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, emitter.getMaterial(), ModelShader::Effect);
        replaceRenderSettings(shader);

        shader.replaceKey("GraphicsEngine::EmitterType", getEmitterDefines(emitter));
    };

    shader_storage.add(emitter.getEmitterType(), compile(SHADER_DIR "effects" PATH_SEPARATOR "sprite_emitter", props));
}

void EffectCompiler::compile(const MeshEmitter& emitter) {
    const auto props = [&] (Shader& shader) {
        replaceMaterialSettings(shader, emitter.getMaterial(), ModelShader::Effect);
        replaceRenderSettings(shader);

        shader.replaceKey("GraphicsEngine::EmitterType", getEmitterDefines(emitter));
    };

    shader_storage.add(emitter.getEmitterType(), compile(SHADER_DIR "effects" PATH_SEPARATOR "mesh_emitter", props));
}

void EffectCompiler::compile(const EffectInstance& instance) {
    for (const auto& [name, emitter] : instance) {
        switch (emitter->getType()) {
            case EmitterType::Sprite:
                compile(instance.get<SpriteEmitter>(name));
                break;
            case EmitterType::Mesh:
                compile(instance.get<MeshEmitter>(name));
                break;
        }
    }
}
