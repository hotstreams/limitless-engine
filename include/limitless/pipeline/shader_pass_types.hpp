#pragma once

#include <set>

namespace Limitless {
    enum class ShaderPass {
        Forward,
        DirectionalShadow,
        Skybox
//        Deferred,
//        ForwardPlus,
//        PointShadow,
//        ColorPicker
    };

    enum class ModelShader {
        Model,
        Skeletal,
        Instanced,
        SkeletalInstanced,
        Effect
    };

    using PassShaders = std::set<ShaderPass>;
    using ModelShaders = std::set<ModelShader>;
}
