#include <limitless/renderer/render_debug_pass.hpp>

#include <limitless/core/context.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/renderer/composite_pass.hpp>
#include <limitless/util/renderer_helper.hpp>
#include <limitless/scene.hpp>
using namespace Limitless;

RenderDebugPass::RenderDebugPass(Pipeline& pipeline, const RendererSettings& settings)
    : RendererPass(pipeline)
    , helper {settings} {
}

void RenderDebugPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets,
                           const Camera &camera,
                           UniformSetter &setter) {
//    if (lighting) {
//        helper.render(ctx, assets, camera, *lighting, instances);
//    }
}

void RenderDebugPass::update(Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    lighting = &scene.getLighting();
}

