#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/gbuffer_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/core/context.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

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

    ctx.enable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Equal);
    ctx.setDepthMask(DepthMask::False);

    auto& fb = renderer.getPass<DeferredFramebufferPass>().getFramebuffer();

    fb.bind();

    instance_renderer.renderScene({ctx, assets,  ShaderType::GBuffer, ms::Blending::Opaque, setter});
}
