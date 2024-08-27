#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/ssr_pass.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/renderer/shader_type.hpp>
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
#include <limitless/renderer/gbuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>

using namespace Limitless;

SSRPass::SSRPass(Pipeline& pipeline, Context& ctx)
    : RendererPass(pipeline)
    , ssr {ctx} {
}

SSRPass::SSRPass(Pipeline& pipeline, Context& ctx, glm::uvec2 frame_size)
    : RendererPass(pipeline)
    , ssr {ctx, frame_size} {
}

void SSRPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
                   UniformSetter &setter) {
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
void SSRPass::addUniformSetter(UniformSetter &setter) {
    setter.add([&](ShaderProgram& shader){
        shader.setUniform("_ssr_texture", getResult());
        shader.setUniform("_ssr_strength", 1.0f);
    });

    ssr.addSetter(setter);
}

