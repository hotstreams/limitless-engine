#include <limitless/pipeline/postprocessing_pass.hpp>

using namespace Limitless;

PostEffectsPass::PostEffectsPass(RenderPass* prev, ContextEventObserver& context)
    : RenderPass(prev)
    , postprocess {context} {
}

void PostEffectsPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] const UniformSetter& setter) {
    postprocess.process(ctx, assets, dynamic_cast<const Framebuffer&>(prev_pass->getTarget()));
}

PostEffectsPass::PostEffectsPass(RenderPass* prev, ContextEventObserver& context, RenderTarget& target)
	: RenderPass(prev)
	, postprocess {context, target} {

}
