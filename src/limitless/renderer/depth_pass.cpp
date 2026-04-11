#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/depth_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/renderer/shader_type.hpp>
// #include <limitless/renderer/indirect_instance_renderer.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>

#include <limitless/fx/effect_renderer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/cpu_profiler.hpp>
#include <limitless/core/state_verifier.hpp>

#include "limitless/core/profiler.hpp"

using namespace Limitless;

DepthPass::DepthPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void DepthPass::render(
        InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        const Camera &camera,
        UniformSetter &setter) {
    ProfilerScope profile_scope {"DepthPass"};

    CpuProfileScope scope(global_profiler, "DepthPass::render");

    ctx.enable(Capabilities::DepthTest);
	ctx.enable(Capabilities::StencilTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);
    ctx.setStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
	ctx.setStencilFunc(StencilFunc::Always, 1, 0xFF);

	auto& fb = renderer.getPass<DeferredFramebufferPass>().getFramebuffer();
    fb.bind();

    DrawParameters drawp {ctx, assets, ShaderType::Depth, ms::Blending::Opaque, setter};

#if 0 // indirect draw disabled
    if (renderer.getSettings().indirect_draw) {
        renderer.getIndirectInstanceRenderer().render(drawp);
        instance_renderer.renderVisibleNonBatchedNonTerrain(drawp);
    } else
#endif
    if (renderer.getSettings().sorted_rendering) {
        instance_renderer.prepareSortedRendering(camera, renderer.getSettings());
        instance_renderer.renderSceneSorted(drawp);
    } else {
        instance_renderer.renderScene(drawp);
    }

    // DEBUG: Verify state after rendering
//    StateVerifier::verifyOrDie("DepthPass::after_render");

	ctx.setStencilMask(0x00);
}
