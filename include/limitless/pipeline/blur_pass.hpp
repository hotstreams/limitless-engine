#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/postprocessing.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class BlurPass final : public RenderPass {
    private:
        Blur blur;
    public:
        BlurPass(Pipeline& pipeline, glm::uvec2 frame_size);

        auto& getBlurResult() { return blur.getResult(); }

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}