#pragma once

#include <set>

namespace Limitless {
    // shader passes depend on material type
    enum class ShaderPass {
        Forward,

        Depth,
        GBuffer,
        Transparent,

        Skybox,
        DirectionalShadow,
    };

    enum class ModelShader {
        Model,
        Skeletal,
        Instanced,
        Effect
    };

    using PassShaders = std::set<ShaderPass>;
    using ModelShaders = std::set<ModelShader>;
}
