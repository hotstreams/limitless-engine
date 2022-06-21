#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class CompositePass final : public RenderPass {
    private:
        Framebuffer framebuffer;
    public:
        explicit CompositePass(Pipeline& pipeline, glm::uvec2 size);

        std::shared_ptr<Texture> getResult() override;

        void onFramebufferChange(glm::uvec2 size) override;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}