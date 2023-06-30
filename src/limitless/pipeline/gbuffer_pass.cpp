#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/gbuffer_pass.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <iostream>

using namespace Limitless;

GBufferPass::GBufferPass(Pipeline& pipeline, fx::EffectRenderer& _renderer)
    : RenderPass(pipeline)
    , renderer {_renderer} {

}

void GBufferPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});

    ctx.enable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Equal);
    ctx.setDepthMask(DepthMask::False);

    auto& fb = pipeline.get<DeferredFramebufferPass>().getFramebuffer();

    fb.bind();

    for (auto& instance : instances) {
        instance.get().draw(ctx, assets, ShaderPass::GBuffer, ms::Blending::Opaque, setter);
    }

    renderer.draw(ctx, assets, ShaderPass::GBuffer, ms::Blending::Opaque, setter);
}
