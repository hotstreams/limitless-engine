#pragma once

#include <set>

namespace Limitless {
    // shader passes depend on material type
    enum class ShaderPass {
        Forward,

        Depth,
        GBuffer,

        Skybox,
        DirectionalShadow,

        ColorPicker
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
