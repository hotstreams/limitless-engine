#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include "postprocessing.hpp"

namespace Limitless {
    class SSAOPass final : public RenderPass {
    private:
        struct SSAOData {
            static constexpr auto KERNEL_COUNT = 64u;

            uint32_t NOISE_SIZE = 4u;
            uint32_t KERNEL_SAMPLES_COUNT = KERNEL_COUNT;

            float radius = 3.0f;
            float bias = 2e-3f;

            std::array<glm::vec4, KERNEL_COUNT> kernel;
        } data;

        Framebuffer framebuffer;
        std::shared_ptr<Texture> noise;
        std::shared_ptr<Buffer> buffer;

        void generateNoise();
        void generateKernel(Context& ctx);
    public:
        SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx);
        ~SSAOPass() override = default;

        std::shared_ptr<Texture> getResult() override { return framebuffer.get(FramebufferAttachment::Color1).texture; }

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}
