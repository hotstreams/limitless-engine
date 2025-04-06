#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/depth_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>

#include <limitless/fx/effect_renderer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>

using namespace Limitless;

DepthPass::DepthPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void DepthPass::render(
        InstanceRenderer& instance_renderer,
        Scene &scene,
        Context &ctx,
        [[maybe_unused]] const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {
    ProfilerScope profile_scope {"DepthPass"};

    ctx.enable(Capabilities::DepthTest);
	ctx.enable(Capabilities::StencilTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);
    ctx.setStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
	ctx.setStencilFunc(StencilFunc::Always, 1, 0xFF);

	auto& fb = renderer.getPass<DeferredFramebufferPass>().getFramebuffer();
    fb.bind();

    instance_renderer.renderScene({ctx, assets, ShaderType::Depth, ms::Blending::Opaque, setter});

	ctx.setStencilMask(0x00);
}
