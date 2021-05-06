#include <limitless/pipeline/render_pass.hpp>

#include <stdexcept>

using namespace Limitless;

RenderPass::RenderPass(RenderPass *pass) noexcept
    : prev_pass {pass} {
}

RenderTarget& RenderPass::getTarget() {
    return prev_pass ? prev_pass->getTarget() : throw std::logic_error("Forgot to add FramebufferPass!");
}

void RenderPass::addSetter([[maybe_unused]] UniformSetter& setter) {

}

void RenderPass::update([[maybe_unused]] Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {

}

void RenderPass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] const UniformSetter& setter) {

}
