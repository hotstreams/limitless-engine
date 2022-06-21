#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/bloom.hpp>

namespace Limitless {
    class BloomPass final : public RenderPass {
    private:
        Bloom bloom;
    public:
        BloomPass(Pipeline& pipeline, glm::uvec2 frame_size);

        auto& getBloom() noexcept { return bloom; }

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        std::shared_ptr<Texture> getResult() override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}