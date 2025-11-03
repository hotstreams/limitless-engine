#pragma once

#include <limitless/ms/material_shader_define_replacer.hpp>
#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <set>

namespace Limitless::ms {
    class Material;
}

namespace Limitless::fx {
    enum class ModuleType;

    class EffectShaderDefineReplacer : public ms::MaterialShaderDefineReplacer {
    private:
        static std::string getEmitterDefines(const AbstractEmitter& emitter) noexcept;
        static std::string getEffectMaterialDependentDefine(const ms::Material& material, InstanceType model_shader, const AbstractEmitter& emitter);
        
        // Vertex stream generation
        static std::string getEffectVertexStreamDeclaration(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        static std::string getEffectVertexStreamGetters(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        
        static std::string getSpriteEmitterVertexStream(const std::set<ModuleType>& modules);
        static std::string getSpriteEmitterVertexGetters(const std::set<ModuleType>& modules);
        
        static std::string getBeamEmitterVertexStream(const std::set<ModuleType>& modules);
        static std::string getBeamEmitterVertexGetters(const std::set<ModuleType>& modules);
        
        static std::string getMeshEmitterVertexStream(const std::set<ModuleType>& modules);
        static std::string getMeshEmitterVertexGetters(const std::set<ModuleType>& modules);
        
        // Vertex context and interface blocks
        static std::string getEffectVertexContext(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        static std::string getEffectVertexContextAssignment(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        static std::string getEffectInterfaceBlockOut(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        static std::string getEffectVertexPassThrough(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        
        // Fragment shader components
        static std::string getEffectInterfaceBlockIn(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        static std::string getEffectInterfaceBlockInGetters(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        static std::string getEffectFragmentContext(AbstractEmitter::Type emitter_type, const std::set<ModuleType>& modules);
        
    public:
        static void replaceMaterialDependentDefine(Shader& shader, const ms::Material& material, InstanceType model_shader, const AbstractEmitter& emitter);
    };
}