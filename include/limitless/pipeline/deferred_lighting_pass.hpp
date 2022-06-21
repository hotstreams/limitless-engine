#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class DeferredLightingPass final : public RenderPass {
    private:
        Framebuffer framebuffer;
    public:
        explicit DeferredLightingPass(Pipeline& pipeline, glm::uvec2 frame_size);

        auto& getFramebuffer() noexcept { return framebuffer; }

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        void onFramebufferChange(glm::uvec2 size) override;

        std::shared_ptr<Texture> getResult() override;
    };
}
