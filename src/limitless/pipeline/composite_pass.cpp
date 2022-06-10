#include <limitless/pipeline/composite_pass.hpp>

#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <limitless/core/texture_builder.hpp>

using namespace Limitless;

CompositePass::CompositePass(Pipeline& pipeline)
    : RenderPass(pipeline) {
}

std::shared_ptr<Texture> CompositePass::getResult() {
	return pipeline.get<DeferredFramebufferPass>().getComposite();
}

void CompositePass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    auto& fb = pipeline.get<DeferredFramebufferPass>().getFramebuffer();

    {
        ctx.setViewPort(ctx.getSize());

		fb.drawBuffer(FramebufferAttachment::Color5);

        auto& shader = assets.shaders.get("composite");

        shader << UniformSampler{"lightened", pipeline.get<DeferredFramebufferPass>().getShaded()};
        shader << UniformSampler{"emissive", pipeline.get<BlurPass>().getBlurResult()};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}
