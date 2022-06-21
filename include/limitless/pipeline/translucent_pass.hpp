#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class TranslucentPass final : public RenderPass {
    private:
        Framebuffer framebuffer;
        fx::EffectRenderer& renderer;
        void sort(Instances& instances, const Camera& camera, ms::Blending blending);
    public:
        explicit TranslucentPass(Pipeline& pipeline, fx::EffectRenderer& renderer, glm::uvec2 frame_size);

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        std::shared_ptr<Texture> getResult() override;
        void onFramebufferChange(glm::uvec2 size) override;
    };
}