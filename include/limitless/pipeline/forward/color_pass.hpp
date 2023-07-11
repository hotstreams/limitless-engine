#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>

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

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}