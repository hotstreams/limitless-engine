#include <limitless/pipeline/pipeline.hpp>

#include <limitless/scene.hpp>
#include "limitless/core/uniform/uniform_setter.hpp"
#include <limitless/core/framebuffer.hpp>
#include <limitless/pipeline/common/quad_pass.hpp>

using namespace Limitless;

Pipeline::Pipeline(glm::uvec2 size, RenderTarget& target) noexcept
    : target {&target}
    , size {size} {
}

void Pipeline::draw(InstanceRenderer& renderer, Context& context, const Assets& assets, Scene& scene, Camera& camera) {
    Instances instances;

    renderer.update(scene, camera);

    for (const auto& pass : passes) {
        pass->update(scene, instances, context, camera);
    }

    UniformSetter setter;
    for (const auto& pass : passes) {
        pass->draw(renderer, scene, context, assets, camera, setter);
        pass->addSetter(setter);
    }
}

void Pipeline::update([[maybe_unused]] Context& ctx, [[maybe_unused]] const RendererSettings& settings) {
}

void Pipeline::clear() {
    passes.clear();
}

void Pipeline::onFramebufferChange(glm::uvec2 frame_size) {
    size = frame_size;

    for (const auto& pass : passes) {
        pass->onFramebufferChange(size);
    }

    target->onFramebufferChange(size);
}

void Pipeline::setTarget(RenderTarget& _target) noexcept {
    target = &_target;

    // setting last quad pass RT to target
    get<ScreenPass>().setTarget(_target);
}

RenderTarget& Pipeline::getTarget() noexcept {
    return *target;
}

std::unique_ptr<PipelinePass>& Pipeline::getPrevious(PipelinePass* curr) {
    for (uint32_t i = 1; i < passes.size(); ++i) {
        if (passes[i].get() == curr) {
            return passes[i - 1];
        }
    }

    throw pipeline_pass_not_found {"There is no previous pass!"};
}

RendererPipeline::Builder& RendererPipeline::Builder::addSceneUpdatePass() {

    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addDirectionalShadowPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addDeferredFramebufferPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addDepthPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addGBufferPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addDecalPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addSkyboxPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addSSAOPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addSSRPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addDeferredLightingPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addTranslucentPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addBloomPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addOutlinePass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addCompositePass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addFXAAPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addScreenPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addRenderDebugPass() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addBefore() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addAfter() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::addAt() {
    return <#initializer#>;
}

RendererPipeline::Builder &RendererPipeline::Builder::deferred() {
    return <#initializer#>;
}

RendererPipeline RendererPipeline::Builder::build() {
    return RendererPipeline();
}
