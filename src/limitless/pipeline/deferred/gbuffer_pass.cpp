#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/deferred/gbuffer_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/core/context.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>

using namespace Limitless;

GBufferPass::GBufferPass(Pipeline& pipeline)
    : PipelinePass(pipeline) {
}

void GBufferPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    ctx.enable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Equal);
    ctx.setDepthMask(DepthMask::False);

    auto& fb = pipeline.get<DeferredFramebufferPass>().getFramebuffer();

    fb.bind();

    renderer.renderScene({ctx, assets,  ShaderType::GBuffer, ms::Blending::Opaque, setter});
}
