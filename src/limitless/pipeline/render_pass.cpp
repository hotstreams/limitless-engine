#include <limitless/pipeline/render_pass.hpp>

#include <stdexcept>

using namespace Limitless;

RenderPass::RenderPass(Pipeline& _pipeline) noexcept
    : pipeline {_pipeline} {
}

void RenderPass::addSetter([[maybe_unused]] UniformSetter& setter) {

}

void RenderPass::update([[maybe_unused]] Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {

}

void RenderPass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {

}
