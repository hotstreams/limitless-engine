#include <limitless/pipeline/forward/postprocessing_pass.hpp>
#include <limitless/renderer/instance_renderer.hpp>

using namespace Limitless;

PostEffectsPass::PostEffectsPass(Pipeline& pipeline, Context& context)
    : PipelinePass(pipeline)
    , postprocess {context.getSize()} {
}

void PostEffectsPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets,
                           const Camera &camera,
                           UniformSetter &setter) {
    //TODO:
    //    postprocess.process(ctx, assets, dynamic_cast<const Framebuffer&>(prev_pass->getTarget()));
}

PostEffectsPass::PostEffectsPass(Pipeline& pipeline, Context& context, RenderTarget& target)
	: PipelinePass(pipeline)
	, postprocess {context.getSize(), target} {

}
