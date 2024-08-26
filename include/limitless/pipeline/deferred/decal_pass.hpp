#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class DecalPass final : public PipelinePass {
    public:
        explicit DecalPass(Pipeline& pipeline);
        ~DecalPass() override = default;

        /**
         * Fills GBUFFER with opaque objects and effects data
         */
        void draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}