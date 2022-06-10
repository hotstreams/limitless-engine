#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class CompositePass final : public RenderPass {
    public:
        explicit CompositePass(Pipeline& pipeline);

        std::shared_ptr<Texture> getResult() override;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}