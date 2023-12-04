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

DepthPass::DepthPass(Pipeline& pipeline, fx::EffectRenderer& _renderer)
    : PipelinePass(pipeline)
    , renderer {_renderer} {
}

void DepthPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    std::sort(instances.begin(), instances.end(), FrontToBackSorter{camera});

    ctx.enable(Capabilities::DepthTest);
	ctx.enable(Capabilities::StencilTest);
    ctx.disable(Capabilities::Blending);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);
    ctx.setStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
	ctx.setStencilFunc(StencilFunc::Always, 1, 0xFF);

	auto& fb = pipeline.get<DeferredFramebufferPass>().getFramebuffer();

    fb.bind();

    for (auto& instance : instances) {
        if (instance.get().getInstanceType() != InstanceType::Decal) {
            instance.get().isOutlined() ? ctx.setStencilMask(0xFF) : ctx.setStencilMask(0x00);
            instance.get().draw(ctx, assets, ShaderType::Depth, ms::Blending::Opaque, setter);
        }
    }

    renderer.draw(ctx, assets, ShaderType::Depth, ms::Blending::Opaque, setter);

	ctx.setStencilMask(0x00);
}
