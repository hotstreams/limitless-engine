#pragma once

#include <limitless/lighting/cascade_shadows.hpp>

namespace LimitlessEngine {
    class ShadowMapping final {
    private:
        CascadeShadows directional_shadow;
    public:
        ShadowMapping();
        ~ShadowMapping() = default;

        void castShadows(Renderer& render, const Assets& assets, Scene& scene, Context& ctx, Camera& camera);

        void setUniform(ShaderProgram& shader) const;
    };
}