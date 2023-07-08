#include <limitless/pipeline/pipeline_pass.hpp>

#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/context.hpp>

#include <stdexcept>

using namespace Limitless;

PipelinePass::PipelinePass(Pipeline& _pipeline) noexcept
    : pipeline {_pipeline} {
}

std::shared_ptr<Texture> PipelinePass::getResult() {
    throw std::logic_error {"This PipelinePass does not provide result method!"};
}

std::shared_ptr<Texture> PipelinePass::getPreviousResult() {
    return pipeline.getPrevious(this)->getResult();
}

void PipelinePass::addSetter([[maybe_unused]] UniformSetter& setter) {

}

void PipelinePass::update([[maybe_unused]] Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {

}

void PipelinePass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {

}

void PipelinePass::onFramebufferChange([[maybe_unused]] glm::uvec2 size) {

}
