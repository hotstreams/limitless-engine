#include <limitless/fx/effect_shader_define_replacer.hpp>

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/modules/module.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/renderer/renderer_settings.hpp>

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

std::string EffectShaderDefineReplacer::getSpriteEmitterVertexStream(const std::set<ModuleType>& modules) {
    std::string stream;
    
    // IMPORTANT: C++ EmitterRenderer ALWAYS sets up ALL attributes (0-6) unconditionally
    // The SpriteParticle struct always has all fields, so we must declare all vertex inputs
    // Only the getters/usage should be conditional based on modules
    
    // Fixed locations to match C++ EmitterRenderer setup (sprite_emitter_renderer.hpp)
    stream += "layout(location = 0) in vec4 vertex_color;\n";
    stream += "layout(location = 1) in vec4 vertex_subUV;\n";
    stream += "layout(location = 2) in vec4 vertex_properties;\n";
    stream += "layout(location = 3) in vec4 vertex_acceleration_lifetime;\n";
    stream += "layout(location = 4) in vec4 vertex_position_size;\n";
    stream += "layout(location = 5) in vec4 vertex_rotation_time;\n";
    stream += "layout(location = 6) in vec4 vertex_velocity;\n";
    
    return stream;
}

std::string EffectShaderDefineReplacer::getSpriteEmitterVertexGetters(const std::set<ModuleType>& modules) {
    std::string getters;
    
    // All data is always present in the vertex buffer, so provide all getters
    getters += "vec4 getParticleColor() {\n    return vertex_color;\n}\n";
    getters += "vec4 getParticleSubUV() {\n    return vertex_subUV;\n}\n";
    getters += "vec4 getParticleProperties() {\n    return vertex_properties;\n}\n";
    getters += "vec3 getParticleAcceleration() {\n    return vertex_acceleration_lifetime.xyz;\n}\n";
    getters += "float getParticleLifetime() {\n    return vertex_acceleration_lifetime.w;\n}\n";
    getters += "vec3 getVertexPosition() {\n    return vertex_position_size.xyz;\n}\n";
    getters += "float getParticleSize() {\n    return vertex_position_size.w;\n}\n";
    getters += "vec3 getParticleRotation() {\n    return vertex_rotation_time.xyz;\n}\n";
    getters += "float getParticleTime() {\n    return vertex_rotation_time.w;\n}\n";
    getters += "vec3 getParticleVelocity() {\n    return vertex_velocity.xyz;\n}\n";
    
    return getters;
}

std::string EffectShaderDefineReplacer::getBeamEmitterVertexStream(const std::set<ModuleType>& modules) {
    std::string stream;
    
    // IMPORTANT: C++ EmitterRenderer ALWAYS sets up ALL attributes (0-9) unconditionally
    // The BeamParticleMapping struct always has all fields, so we must declare all vertex inputs
    
    // Fixed locations to match C++ EmitterRenderer setup (beam_emitter_renderer.hpp)
    stream += "layout(location = 0) in vec4 vertex_position;\n";
    stream += "layout(location = 1) in vec4 vertex_color;\n";
    stream += "layout(location = 2) in vec4 vertex_subUV;\n";
    stream += "layout(location = 3) in vec4 vertex_properties;\n";
    stream += "layout(location = 4) in vec4 vertex_acceleration_lifetime;\n";
    stream += "layout(location = 5) in vec4 vertex_rotation_time;\n";
    stream += "layout(location = 6) in vec4 vertex_velocity_size;\n";
    stream += "layout(location = 7) in vec4 vertex_uv_length;\n";
    stream += "layout(location = 8) in vec3 vertex_start;\n";
    stream += "layout(location = 9) in vec3 vertex_end;\n";
    
    return stream;
}

std::string EffectShaderDefineReplacer::getBeamEmitterVertexGetters(const std::set<ModuleType>& modules) {
    std::string getters;
    
    // All data is always present in the vertex buffer, so provide all getters
    getters += "vec3 getVertexPosition() {\n    return vertex_position.xyz;\n}\n";
    getters += "vec4 getParticleColor() {\n    return vertex_color;\n}\n";
    getters += "vec4 getParticleSubUV() {\n    return vertex_subUV;\n}\n";
    getters += "vec4 getParticleProperties() {\n    return vertex_properties;\n}\n";
    getters += "vec3 getParticleAcceleration() {\n    return vertex_acceleration_lifetime.xyz;\n}\n";
    getters += "float getParticleLifetime() {\n    return vertex_acceleration_lifetime.w;\n}\n";
    getters += "vec3 getParticleRotation() {\n    return vertex_rotation_time.xyz;\n}\n";
    getters += "float getParticleTime() {\n    return vertex_rotation_time.w;\n}\n";
    getters += "vec3 getParticleVelocity() {\n    return vertex_velocity_size.xyz;\n}\n";
    getters += "float getParticleSize() {\n    return vertex_velocity_size.w;\n}\n";
    getters += "vec2 getVertexUV() {\n    return vertex_uv_length.xy;\n}\n";
    getters += "float getParticleLength() {\n    return vertex_uv_length.z;\n}\n";
    getters += "vec3 getParticleStart() {\n    return vertex_start;\n}\n";
    getters += "vec3 getParticleEnd() {\n    return vertex_end;\n}\n";
    
    return getters;
}

std::string EffectShaderDefineReplacer::getMeshEmitterVertexStream(const std::set<ModuleType>& modules) {
    std::string stream;
    
    // Regular mesh vertex inputs
    stream += "layout(location = 0) in vec3 _vertex_position;\n";
    stream += "layout(location = 1) in vec3 _vertex_normal;\n";
    stream += "#if defined(ENGINE_MATERIAL_NORMAL_TEXTURE) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)\n";
    stream += "    layout(location = 2) in vec3 _vertex_tangent;\n";
    stream += "#endif\n";
    stream += "layout(location = 3) in vec2 _vertex_uv;\n";
    stream += "#if defined(ENGINE_MATERIAL_SKELETAL_MODEL)\n";
    stream += "    layout(location = 4) in ivec4 _vertex_bone_id;\n";
    stream += "    layout(location = 5) in vec4 _vertex_bone_weight;\n";
    stream += "#endif\n";
    
    return stream;
}

std::string EffectShaderDefineReplacer::getMeshEmitterVertexGetters(const std::set<ModuleType>& modules) {
    std::string getters;
    
    // Mesh vertex getters
    getters += "vec3 getVertexPosition() {\n    return _vertex_position;\n}\n";
    getters += "vec3 getVertexNormal() {\n    return _vertex_normal;\n}\n";
    getters += "vec2 getVertexUV() {\n    return _vertex_uv;\n}\n";
    getters += "#if defined(ENGINE_MATERIAL_NORMAL_TEXTURE) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)\n";
    getters += "vec3 getVertexTangent() {\n    return _vertex_tangent;\n}\n";
    getters += "#endif\n";
    getters += "#if defined(ENGINE_MATERIAL_SKELETAL_MODEL)\n";
    getters += "ivec4 getVertexBoneID() {\n    return _vertex_bone_id;\n}\n";
    getters += "vec4 getVertexBoneWeight() {\n    return _vertex_bone_weight;\n}\n";
    getters += "#endif\n";
    
    // SSBO particle structure
    getters += "struct MeshParticle {\n";
    getters += "    mat4 model;\n";
    getters += "    vec4 color;\n";
    getters += "    vec4 subUV;\n";
    getters += "    vec4 properties;\n";
    getters += "    vec4 acceleration_lifetime;\n";
    getters += "    vec4 position;\n";
    getters += "    vec4 rotation_time;\n";
    getters += "    vec4 velocity;\n";
    getters += "    vec4 size;\n";
    getters += "};\n";
    getters += "layout(std430) buffer mesh_emitter_particles {\n";
    getters += "    MeshParticle _particles[];\n";
    getters += "};\n";
    
    // Particle data getters from SSBO
    getters += "mat4 getModelMatrix() {\n    return _particles[gl_InstanceID].model;\n}\n";
    getters += "vec4 getParticleColor() {\n    return _particles[gl_InstanceID].color;\n}\n";
    getters += "vec4 getParticleSubUV() {\n    return _particles[gl_InstanceID].subUV;\n}\n";
    getters += "vec4 getParticleProperties() {\n    return _particles[gl_InstanceID].properties;\n}\n";
    getters += "vec3 getParticleAcceleration() {\n    return _particles[gl_InstanceID].acceleration_lifetime.xyz;\n}\n";
    getters += "float getParticleLifetime() {\n    return _particles[gl_InstanceID].acceleration_lifetime.w;\n}\n";
    getters += "vec3 getParticlePosition() {\n    return _particles[gl_InstanceID].position.xyz;\n}\n";
    getters += "vec3 getParticleSize() {\n    return _particles[gl_InstanceID].size.xyz;\n}\n";
    getters += "vec3 getParticleRotation() {\n    return _particles[gl_InstanceID].rotation_time.xyz;\n}\n";
    getters += "float getParticleTime() {\n    return _particles[gl_InstanceID].rotation_time.w;\n}\n";
    getters += "vec3 getParticleVelocity() {\n    return _particles[gl_InstanceID].velocity.xyz;\n}\n";
    
    return getters;
}

std::string EffectShaderDefineReplacer::getEffectVertexStreamDeclaration(
    AbstractEmitter::Type emitter_type,
    const std::set<ModuleType>& modules
) {
    switch (emitter_type) {
        case AbstractEmitter::Type::Sprite:
            return getSpriteEmitterVertexStream(modules);
        case AbstractEmitter::Type::Beam:
            return getBeamEmitterVertexStream(modules);
        case AbstractEmitter::Type::Mesh:
            return getMeshEmitterVertexStream(modules);
        default:
            throw std::runtime_error("Unknown emitter type in getEffectVertexStreamDeclaration");
    }
}

std::string EffectShaderDefineReplacer::getEffectVertexStreamGetters(
    AbstractEmitter::Type emitter_type,
    const std::set<ModuleType>& modules
) {
    switch (emitter_type) {
        case AbstractEmitter::Type::Sprite:
            return getSpriteEmitterVertexGetters(modules);
        case AbstractEmitter::Type::Beam:
            return getBeamEmitterVertexGetters(modules);
        case AbstractEmitter::Type::Mesh:
            return getMeshEmitterVertexGetters(modules);
        default:
            throw std::runtime_error("Unknown emitter type in getEffectVertexStreamGetters");
    }
}

std::string EffectShaderDefineReplacer::getEffectMaterialDependentDefine(const ms::Material& material, InstanceType model_shader, const AbstractEmitter& emitter) {
    // For effects, we only need material defines and emitter defines
    // We don't use getModelDefines because effects have custom vertex streams
    std::string define = getMaterialDefines(material);
    define.append("#define " + MODEL_DEFINE.at(model_shader) + '\n');
    
    // Effects always have UV (from gl_PointCoord for sprites, or vertex data for mesh/beam)
    define.append("#define ENGINE_VERTEX_UV\n");
    
    // For mesh emitters, add normal
    if (emitter.getType() == AbstractEmitter::Type::Mesh) {
        define.append("#define ENGINE_VERTEX_NORMAL\n");
    }
    
    define.append(getEmitterDefines(emitter));
    return define;
}

std::string EffectShaderDefineReplacer::getEffectVertexContext(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    // For effects, VertexContext is minimal - just position
    return "struct VertexContext {\n    vec3 position;\n};\n";
}

std::string EffectShaderDefineReplacer::getEffectVertexContextAssignment(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    // For effects, just assign position
    return "vctx.position = getVertexPosition();\n";
}

std::string EffectShaderDefineReplacer::getEffectInterfaceBlockOut(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    std::string block = "out _vertex_data {\n";
    
    // Add fields based on modules and emitter type
    if (modules.count(ModuleType::InitialColor)) {
        block += "    vec4 color;\n";
    }
    
    if (modules.count(ModuleType::SubUV)) {
        block += "    vec4 subUV;\n";
    }
    
    if (modules.count(ModuleType::CustomMaterial)) {
        block += "    vec4 properties;\n";
    }
    
    if (modules.count(ModuleType::Lifetime) || modules.count(ModuleType::InitialAcceleration)) {
        block += "    vec3 acceleration;\n";
        block += "    float lifetime;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        block += "#if defined(ENGINE_MATERIAL_NORMAL_TEXTURE) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)\n";
        block += "    vec3 tangent;\n";
        block += "    vec3 normal;\n";
        block += "#else\n";
        block += "    vec3 normal;\n";
        block += "#endif\n";
    }
    
    // Always include world_position
    block += "    vec3 world_position;\n";
    
    if (emitter_type == AbstractEmitter::Type::Sprite) {
        block += "    float size;\n";
    }
    
    if (modules.count(ModuleType::InitialRotation) || modules.count(ModuleType::Time)) {
        block += "    vec3 rotation;\n";
        block += "    float time;\n";
    }
    
    if (modules.count(ModuleType::InitialVelocity)) {
        if (emitter_type == AbstractEmitter::Type::Sprite || emitter_type == AbstractEmitter::Type::Mesh) {
            block += "    vec3 velocity;\n";
        } else if (emitter_type == AbstractEmitter::Type::Beam) {
            block += "    vec3 velocity;\n";
            if (modules.count(ModuleType::InitialSize)) {
                block += "    float size;\n";
            }
        }
    } else if (emitter_type == AbstractEmitter::Type::Beam && modules.count(ModuleType::InitialSize)) {
        block += "    float size;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        block += "    vec3 size;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Beam || emitter_type == AbstractEmitter::Type::Mesh) {
        block += "    vec2 uv;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Beam && modules.count(ModuleType::BeamSpeed)) {
        block += "    vec3 start;\n";
        block += "    vec3 end;\n";
        block += "    float length;\n";
    }
    
    block += "} _out_data;\n";
    return block;
}

std::string EffectShaderDefineReplacer::getEffectVertexPassThrough(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    std::string passthrough;
    
    // Pass world position
    passthrough += "_out_data.world_position = ectx.world_position.xyz;\n";
    
    // For sprite emitters, set gl_PointSize
    if (emitter_type == AbstractEmitter::Type::Sprite) {
        if (modules.count(ModuleType::InitialSize)) {
            passthrough += "gl_PointSize = getProjection()[1][1] * getParticleSize() / gl_Position.w;\n";
        } else {
            passthrough += "gl_PointSize = getProjection()[1][1] * 16.0 / gl_Position.w;\n";
        }
    }
    
    // Pass module-specific data
    if (modules.count(ModuleType::InitialColor)) {
        passthrough += "_out_data.color = getParticleColor();\n";
    }
    
    if (modules.count(ModuleType::SubUV)) {
        passthrough += "_out_data.subUV = getParticleSubUV();\n";
    }
    
    if (modules.count(ModuleType::CustomMaterial)) {
        passthrough += "_out_data.properties = getParticleProperties();\n";
    }
    
    if (modules.count(ModuleType::InitialAcceleration)) {
        passthrough += "_out_data.acceleration = getParticleAcceleration();\n";
    }
    
    if (modules.count(ModuleType::Lifetime)) {
        passthrough += "_out_data.lifetime = getParticleLifetime();\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Sprite) {
        passthrough += "_out_data.size = getParticleSize();\n";
    }
    
    if (modules.count(ModuleType::InitialRotation)) {
        passthrough += "_out_data.rotation = getParticleRotation();\n";
    }
    
    if (modules.count(ModuleType::Time)) {
        passthrough += "_out_data.time = getParticleTime();\n";
    }
    
    if (modules.count(ModuleType::InitialVelocity)) {
        passthrough += "_out_data.velocity = getParticleVelocity();\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Beam || emitter_type == AbstractEmitter::Type::Mesh) {
        passthrough += "_out_data.uv = getVertexUV();\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Beam) {
        if (modules.count(ModuleType::InitialSize)) {
            passthrough += "_out_data.size = getParticleSize();\n";
        }
        if (modules.count(ModuleType::BeamSpeed)) {
            passthrough += "_out_data.start = getParticleStart();\n";
            passthrough += "_out_data.end = getParticleEnd();\n";
            passthrough += "_out_data.length = getParticleLength();\n";
        }
    }
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        passthrough += "_out_data.size = getParticleSize();\n";
        passthrough += "_out_data.normal = getVertexNormal();\n";
        passthrough += "#if defined(ENGINE_MATERIAL_NORMAL_TEXTURE) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)\n";
        passthrough += "_out_data.tangent = getVertexTangent();\n";
        passthrough += "#endif\n";
    }
    
    return passthrough;
}

std::string EffectShaderDefineReplacer::getEffectInterfaceBlockIn(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    // Same structure as output block but with 'in' instead of 'out'
    std::string block = "in _vertex_data {\n";
    
    if (modules.count(ModuleType::InitialColor)) {
        block += "    vec4 color;\n";
    }
    
    if (modules.count(ModuleType::SubUV)) {
        block += "    vec4 subUV;\n";
    }
    
    if (modules.count(ModuleType::CustomMaterial)) {
        block += "    vec4 properties;\n";
    }
    
    if (modules.count(ModuleType::Lifetime) || modules.count(ModuleType::InitialAcceleration)) {
        block += "    vec3 acceleration;\n";
        block += "    float lifetime;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        block += "#if defined(ENGINE_MATERIAL_NORMAL_TEXTURE) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)\n";
        block += "    vec3 tangent;\n";
        block += "    vec3 normal;\n";
        block += "#else\n";
        block += "    vec3 normal;\n";
        block += "#endif\n";
    }
    
    block += "    vec3 world_position;\n";
    
    if (emitter_type == AbstractEmitter::Type::Sprite) {
        block += "    float size;\n";
    }
    
    if (modules.count(ModuleType::InitialRotation) || modules.count(ModuleType::Time)) {
        block += "    vec3 rotation;\n";
        block += "    float time;\n";
    }
    
    if (modules.count(ModuleType::InitialVelocity)) {
        if (emitter_type == AbstractEmitter::Type::Sprite || emitter_type == AbstractEmitter::Type::Mesh) {
            block += "    vec3 velocity;\n";
        } else if (emitter_type == AbstractEmitter::Type::Beam) {
            block += "    vec3 velocity;\n";
            if (modules.count(ModuleType::InitialSize)) {
                block += "    float size;\n";
            }
        }
    } else if (emitter_type == AbstractEmitter::Type::Beam && modules.count(ModuleType::InitialSize)) {
        block += "    float size;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        block += "    vec3 size;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Beam || emitter_type == AbstractEmitter::Type::Mesh) {
        block += "    vec2 uv;\n";
    }
    
    if (emitter_type == AbstractEmitter::Type::Beam && modules.count(ModuleType::BeamSpeed)) {
        block += "    vec3 start;\n";
        block += "    vec3 end;\n";
        block += "    float length;\n";
    }
    
    block += "} _in_data;\n";
    return block;
}

std::string EffectShaderDefineReplacer::getEffectInterfaceBlockInGetters(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    std::string getters;
    
    // Beam speed getters
    if (emitter_type == AbstractEmitter::Type::Beam && modules.count(ModuleType::BeamSpeed)) {
        getters += "float getParticleLength() {\n    return _in_data.length;\n}\n";
        getters += "vec3 getParticleStart() {\n    return _in_data.start;\n}\n";
        getters += "vec3 getParticleEnd() {\n    return _in_data.end;\n}\n";
    }
    
    // Module-specific getters
    if (modules.count(ModuleType::InitialColor)) {
        getters += "vec4 getParticleColor() {\n    return _in_data.color;\n}\n";
    }
    
    if (modules.count(ModuleType::SubUV)) {
        getters += "vec4 getParticleSubUV() {\n    return _in_data.subUV;\n}\n";
    }
    
    if (modules.count(ModuleType::CustomMaterial)) {
        getters += "vec4 getParticleProperties() {\n    return _in_data.properties;\n}\n";
    }
    
    if (modules.count(ModuleType::Lifetime) || modules.count(ModuleType::InitialAcceleration)) {
        if (modules.count(ModuleType::InitialAcceleration)) {
            getters += "vec3 getParticleAcceleration() {\n    return _in_data.acceleration;\n}\n";
        }
        if (modules.count(ModuleType::Lifetime)) {
            getters += "float getParticleLifetime() {\n    return _in_data.lifetime;\n}\n";
        }
    }
    
    // World position getter (always available)
    getters += "vec3 getVertexPosition() {\n    return _in_data.world_position;\n}\n";
    
    // Size getters
    if (emitter_type == AbstractEmitter::Type::Sprite) {
        getters += "float getParticleSize() {\n    return _in_data.size;\n}\n";
    } else if (emitter_type == AbstractEmitter::Type::Beam && modules.count(ModuleType::InitialSize)) {
        getters += "float getParticleSize() {\n    return _in_data.size;\n}\n";
    } else if (emitter_type == AbstractEmitter::Type::Mesh) {
        getters += "vec3 getParticleSize() {\n    return _in_data.size;\n}\n";
    }
    
    // Rotation and time getters
    if (modules.count(ModuleType::InitialRotation) || modules.count(ModuleType::Time)) {
        if (modules.count(ModuleType::InitialRotation)) {
            getters += "vec3 getParticleRotation() {\n    return _in_data.rotation;\n}\n";
        }
        if (modules.count(ModuleType::Time)) {
            getters += "float getParticleTime() {\n    return _in_data.time;\n}\n";
        }
    }
    
    // Velocity getters
    if (modules.count(ModuleType::InitialVelocity)) {
        if (emitter_type == AbstractEmitter::Type::Sprite || emitter_type == AbstractEmitter::Type::Mesh) {
            getters += "vec3 getParticleVelocity() {\n    return _in_data.velocity;\n}\n";
        } else if (emitter_type == AbstractEmitter::Type::Beam) {
            getters += "vec3 getParticleVelocity() {\n    return _in_data.velocity;\n}\n";
        }
    }
    
    // UV getters
    if (emitter_type == AbstractEmitter::Type::Beam || emitter_type == AbstractEmitter::Type::Mesh) {
        getters += "vec2 getVertexUV() {\n    return _in_data.uv;\n}\n";
    } else if (emitter_type == AbstractEmitter::Type::Sprite) {
        getters += "vec2 getVertexUV() {\n";
        getters += "    vec2 uv = gl_PointCoord;\n";
        if (modules.count(ModuleType::InitialRotation)) {
            getters += "#if defined(InitialRotation_MODULE)\n";
            getters += "    vec2 center = vec2(0.5, 0.5);\n";
            getters += "    vec2 translated = uv - center;\n";
            getters += "    mat2 rotation = mat2(cos(getParticleRotation().x), sin(getParticleRotation().x), -sin(getParticleRotation().x), cos(getParticleRotation().x));\n";
            getters += "    translated = rotation * translated;\n";
            getters += "    translated = translated + center;\n";
            getters += "    uv = clamp(translated, 0.0, 1.0);\n";
            getters += "#endif\n";
        }
        if (modules.count(ModuleType::SubUV)) {
            getters += "#if defined(SubUV_MODULE)\n";
            getters += "    uv = uv * getParticleSubUV().xy + getParticleSubUV().zw;\n";
            getters += "#endif\n";
        }
        getters += "    return uv;\n";
        getters += "}\n";
    }
    
    // Mesh emitter normal/tangent getters
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        getters += "#if defined(ENGINE_MATERIAL_NORMAL_TEXTURE) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)\n";
        getters += "vec3 getVertexTangent() {\n    return _in_data.tangent;\n}\n";
        getters += "#endif\n";
        getters += "vec3 getVertexNormal() {\n    return _in_data.normal;\n}\n";
    }
    
    return getters;
}

std::string EffectShaderDefineReplacer::getEffectFragmentContext(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules) {
    // For effects in fragment shader, VertexContext needs position and uv
    std::string context = "struct VertexContext {\n";
    context += "    vec3 position;\n";
    context += "    vec2 uv;\n"; // All emitters need UV (sprites use gl_PointCoord, others use vertex data)
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        context += "    vec3 normal;\n";
    }
    
    context += "};\n\n";
    
    context += "VertexContext computeVertexContext() {\n";
    context += "    VertexContext vctx;\n";
    context += "    vctx.position = getVertexPosition();\n";
    context += "    vctx.uv = getVertexUV();\n"; // Always set UV
    
    if (emitter_type == AbstractEmitter::Type::Mesh) {
        context += "    vctx.normal = getVertexNormal();\n";
    }
    
    context += "    return vctx;\n";
    context += "}\n";
    
    return context;
}

void EffectShaderDefineReplacer::replaceMaterialDependentDefine(Shader& shader, const ms::Material& material, InstanceType model_shader, const AbstractEmitter& emitter) {
    shader.replaceKey(DEFINE_NAMES.at(Define::MaterialDependent), getEffectMaterialDependentDefine(material, model_shader, emitter));

    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Vertex], material.getVertexSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::Fragment], material.getFragmentSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::GlobalVertex], material.getGlobalVertexSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::GlobalFragment], material.getGlobalFragmentSnippet());
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomScalar], getScalarUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomSamplers], getSamplerUniformDefines(material));
    shader.replaceKey(SNIPPET_DEFINE[SnippetDefineType::CustomShading], material.getShadingSnippet());
    
    // Generate runtime vertex streams for effects
    const auto& unique_shader = emitter.getUniqueShaderType();
    std::set<ModuleType> modules(unique_shader.module_type.begin(), unique_shader.module_type.end());
    
    std::string vertex_stream = getEffectVertexStreamDeclaration(emitter.getType(), modules);
    std::string vertex_getters = getEffectVertexStreamGetters(emitter.getType(), modules);
    
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::Stream], vertex_stream + vertex_getters);
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::VertexContext], getEffectVertexContext(emitter.getType(), modules));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::InterfaceBlockOut], getEffectInterfaceBlockOut(emitter.getType(), modules));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::ContextAssignment], getEffectVertexContextAssignment(emitter.getType(), modules));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::PassThrough], getEffectVertexPassThrough(emitter.getType(), modules));
    
    // Fragment shader replacements
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::InterfaceBlockIn], getEffectInterfaceBlockIn(emitter.getType(), modules) + getEffectInterfaceBlockInGetters(emitter.getType(), modules));
    shader.replaceKey(VERTEX_STREAM_DEFINE[VertexDefineType::FragmentContext], getEffectFragmentContext(emitter.getType(), modules));
}
