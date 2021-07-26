#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/util/renderer_helper.hpp>
#include <limitless/lighting/lighting.hpp>

namespace Limitless {
    class RenderDebugPass final : public RenderPass {
    private:
        RendererHelper helper;
        Lighting* lighting {};
    public:
        explicit RenderDebugPass(Pipeline& pipeline, const RenderSettings& settings);
        ~RenderDebugPass() override = default;

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}