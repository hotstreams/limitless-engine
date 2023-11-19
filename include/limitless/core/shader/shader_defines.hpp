#pragma once

#include <unordered_map>
#include <string>

namespace Limitless {
    enum class EngineShaderDefine {
        GLSLVersion,
        Extensions,
        Settings,
        MaterialType,
        ModelType,
        EmitterType,

        // GLSLVersion + Extensions + Settings
        Common,

        // ALL OF ABOVE
        MaterialDependent
    };

    static inline std::unordered_map<EngineShaderDefine, std::string> ENGINE_SHADER_DEFINE_NAMES =
    {
        { EngineShaderDefine::GLSLVersion, "ENGINE::GLSLVERSION"},
        { EngineShaderDefine::Extensions, "ENGINE::EXTENSIONS"},
        { EngineShaderDefine::Settings, "ENGINE::SETTINGS"},
        { EngineShaderDefine::MaterialType, "ENGINE::MATERIALTYPE"},
        { EngineShaderDefine::ModelType, "ENGINE::MODELTYPE"},
        { EngineShaderDefine::EmitterType, "ENGINE::EMITTERTYPE"},
        { EngineShaderDefine::Common, "ENGINE::COMMON"},
        { EngineShaderDefine::MaterialDependent, "ENGINE::MATERIALDEPENDENT"},
    };
}