#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>

namespace Limitless {
    class Framebuffer;

    class FrustumCullingPass final : public PipelinePass {
    public:
        explicit FrustumCullingPass(Pipeline& pipeline);

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
    };
}