#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless::ms {
    enum class Blending;
}

namespace Limitless {
    class ColorPass final : public PipelinePass {
    private:
        ms::Blending blending;
    public:
        explicit ColorPass(Pipeline& pipeline, ms::Blending blending);
        ~ColorPass() override = default;

        void
        draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
             UniformSetter &setter) override;
    };
}