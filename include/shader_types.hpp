#pragma once

namespace LimitlessEngine {
    enum class MaterialShader {
        Default,
        Deferred,
        ForwardPlus,
        DirectionalShadow,
        PointShadow,
        ColorPicker,
        Text
    };

    enum class ModelShader {
        Model,
        Skeletal,
        Instanced,
        SkeletalInstanced,
        Effect,
        Text
    };

    using MaterialShaders = std::vector<MaterialShader>;
    using ModelShaders = std::vector<ModelShader>;
}
