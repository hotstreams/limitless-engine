#include <limitless/pipeline/render_pass.hpp>

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/context.hpp>

#include <stdexcept>

using namespace Limitless;

RenderPass::RenderPass(Pipeline& _pipeline) noexcept
    : pipeline {_pipeline} {
}

void RenderPass::addSetter([[maybe_unused]] UniformSetter& setter) {

}

void RenderPass::update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) {
    update(scene, instances, ctx, ctx.getSize(), camera);
}

void RenderPass::update([[maybe_unused]] Scene& scene,
                        [[maybe_unused]] Instances& instances,
                        [[maybe_unused]] Context& ctx,
                        [[maybe_unused]] glm::uvec2 frame_size,
                        [[maybe_unused]] const Camera& camera) {

}

void RenderPass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {

}

std::shared_ptr<Texture> RenderPass::getPreviousResult() {
	return pipeline.getPrevious(this)->getResult();
}
