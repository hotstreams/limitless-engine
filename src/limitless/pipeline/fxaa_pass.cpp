#include <limitless/pipeline/fxaa_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/composite_pass.hpp>

using namespace Limitless;

FXAAPass::FXAAPass(Pipeline& pipeline)
    : RenderPass(pipeline)
    , target {default_framebuffer} {
}

FXAAPass::FXAAPass(Pipeline& pipeline, RenderTarget& _target)
	: RenderPass(pipeline)
	, target {_target} {
}


void FXAAPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        target.clear();
        auto& shader = assets.shaders.get("fxaa");

        shader << UniformSampler{"scene", pipeline.get<CompositePass>().getResult()};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}
