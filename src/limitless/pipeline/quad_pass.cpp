#include <limitless/pipeline/quad_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/uniform/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/composite_pass.hpp>
#include <limitless/pipeline/deferred_lighting_pass.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
using namespace Limitless;

FinalQuadPass::FinalQuadPass(Pipeline& pipeline)
    : RenderPass(pipeline)
    , target {&default_framebuffer} {
}

FinalQuadPass::FinalQuadPass(Pipeline& pipeline, RenderTarget& _target)
	: RenderPass(pipeline)
	, target {&_target} {
}

void FinalQuadPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
	    target->clear();
        auto& shader = assets.shaders.get("quad");

        shader << UniformSampler{"screen_texture", getPreviousResult()};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void FinalQuadPass::setTarget(RenderTarget& _target) {
    target = &_target;
}
