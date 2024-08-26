#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/deferred/depth_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>

#include <limitless/fx/effect_renderer.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>

using namespace Limitless;

DepthPass::DepthPass(Pipeline& pipeline)
    : PipelinePass(pipeline) {
}

void DepthPass::draw(InstanceRenderer& renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    ctx.enable(Capabilities::DepthTest);
	ctx.enable(Capabilities::StencilTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);
    ctx.setStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
	ctx.setStencilFunc(StencilFunc::Always, 1, 0xFF);

	auto& fb = pipeline.get<DeferredFramebufferPass>().getFramebuffer();
    fb.bind();

    renderer.renderScene({ctx, assets, ShaderType::Depth, ms::Blending::Opaque, setter});

	ctx.setStencilMask(0x00);
}
