#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/gbuffer_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/renderer/renderer.hpp>
// #include <limitless/renderer/indirect_instance_renderer.hpp>
#include <limitless/util/frustum_culling.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/core/context.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/core/cpu_profiler.hpp>
#include <limitless/core/state_verifier.hpp>

#include "limitless/core/profiler.hpp"

using namespace Limitless;

GBufferPass::GBufferPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void GBufferPass::render(
        InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        UniformSetter &setter) {
    ProfilerScope profile_scope {"GBufferPass"};

    ctx.enable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Equal);
    ctx.setDepthMask(DepthMask::False);

    auto& fb = renderer.getPass<DeferredFramebufferPass>().getFramebuffer();

    fb.bind();

    DrawParameters drawp {ctx, assets, ShaderType::GBuffer, ms::Blending::Opaque, setter};

    // Render terrain with minimal MRT set to reduce bandwidth
    renderer.getPass<DeferredFramebufferPass>().getFramebuffer().drawBuffers({
        FramebufferAttachment::Color0, // albedo
        FramebufferAttachment::Color1, // normal
        FramebufferAttachment::Color2  // properties
    });
    if (renderer.getSettings().sorted_rendering) {
        instance_renderer.renderVisibleTerrainOnlySorted(drawp);
    } else {
        instance_renderer.renderVisibleTerrainOnly(drawp);
    }

    // Render the rest with full MRT set
    renderer.getPass<DeferredFramebufferPass>().getFramebuffer().drawBuffers({
        FramebufferAttachment::Color0,
        FramebufferAttachment::Color1,
        FramebufferAttachment::Color2,
        FramebufferAttachment::Color3,
        FramebufferAttachment::Color4,
        FramebufferAttachment::Color5
    });

#if 0
    if (renderer.getSettings().indirect_draw) {
        renderer.getIndirectInstanceRenderer().render(drawp);
        instance_renderer.renderVisibleNonBatchedNonTerrain(drawp);
    } else
#endif
    if (renderer.getSettings().sorted_rendering) {
        instance_renderer.renderVisibleNonTerrainSorted(drawp);
    } else {
        instance_renderer.renderVisibleNonTerrain(drawp);
    }

    // DEBUG: Verify state after rendering
//    StateVerifier::verifyOrDie("GBufferPass::after_render");
}
