#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/common/ssr_pass.hpp>
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

SSRPass::SSRPass(Pipeline& pipeline, Context& ctx)
    : PipelinePass(pipeline)
    , ssr {ctx} {
}

SSRPass::SSRPass(Pipeline& pipeline, Context& ctx, glm::uvec2 frame_size)
    : PipelinePass(pipeline)
    , ssr {ctx, frame_size} {
}

void SSRPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

    ssr.draw(ctx, assets, camera, gbuffer.getDepth(), gbuffer.getNormal(), gbuffer.getProperties(), gbuffer.getAlbedo());
}

void SSRPass::onFramebufferChange(glm::uvec2 size) {
    ssr.onFramebufferChange(size);
}
//
//void SSRPass::update(Scene &scene, Instances &instances, Context &ctx, const Camera &camera) {
//    ssao.update(camera);
//}
//
void SSRPass::addSetter(Limitless::UniformSetter &setter) {
    setter.add([&](ShaderProgram& shader){
        shader.setUniform("_ssr_texture", getResult());
        shader.setUniform("_ssr_strength", 1.0f);
    });

    ssr.addSetter(setter);
}

