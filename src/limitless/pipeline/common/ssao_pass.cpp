#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/common/ssao_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include <random>
#include <iostream>
#include <limitless/camera.hpp>
#include <limitless/pipeline/deferred/gbuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>

using namespace Limitless;

SSAOPass::SSAOPass(Pipeline& pipeline, Context& ctx)
    : PipelinePass(pipeline)
    , ssao {ctx} {
}

SSAOPass::SSAOPass(Pipeline& pipeline, Context& ctx, glm::uvec2 frame_size)
    : PipelinePass(pipeline)
    , ssao {ctx, frame_size} {
}

void SSAOPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
                    UniformSetter &setter) {
    ssao.draw(ctx, assets, pipeline.get<DeferredFramebufferPass>().getDepth());
}

void SSAOPass::onFramebufferChange(glm::uvec2 size) {
    ssao.onFramebufferChange(size);
}

void SSAOPass::update([[maybe_unused]] Scene &scene, [[maybe_unused]] Instances &instances, [[maybe_unused]] Context &ctx, const Camera &camera) {
    ssao.update(camera);
}

void SSAOPass::addSetter(Limitless::UniformSetter &setter) {
    setter.add([&](ShaderProgram& shader){
        shader.setUniform("_ssao_texture", getResult());
    });
}

