#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/fx/effect_renderer.hpp>

namespace Limitless {
    class EffectUpdatePass final : public RenderPass {
    private:
        fx::EffectRenderer renderer;
    public:
        EffectUpdatePass(Pipeline& pipeline, Context& ctx);
        ~EffectUpdatePass() override = default;

        auto& getRenderer() { return renderer; }

        void update(Scene& scene, Instances& instances, Context& ctx, glm::uvec2 frame_size, const Camera& camera) override;
    };
}