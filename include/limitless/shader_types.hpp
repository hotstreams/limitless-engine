#pragma once

#include <vector>

namespace LimitlessEngine {
    enum class MaterialShader {
        Forward,
        Deferred,
        ForwardPlus,
        DirectionalShadow,
        PointShadow,
        ColorPicker
    };

    enum class ModelShader {
        Model,
        Skeletal,
        Instanced,
        SkeletalInstanced,
        Effect
    };

    using MaterialShaders = std::vector<MaterialShader>;
    using ModelShaders = std::vector<ModelShader>;
}
