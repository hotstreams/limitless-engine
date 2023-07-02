#include <limitless/pipeline/common/quad_pass.hpp>

#include <limitless/core/context.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/composite_pass.hpp>
#include <limitless/pipeline/deferred/deferred_lighting_pass.hpp>

using namespace Limitless;

ScreenPass::ScreenPass(Pipeline& pipeline)
    : RenderPass(pipeline)
    , target {&default_framebuffer} {
}

ScreenPass::ScreenPass(Pipeline& pipeline, RenderTarget& _target)
	: RenderPass(pipeline)
	, target {&_target} {
}

void ScreenPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
	    target->clear();
        auto& shader = assets.shaders.get("quad");

        shader.setUniform("screen_texture", getPreviousResult());

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void ScreenPass::setTarget(RenderTarget& _target) {
    target = &_target;
}
