#include <limitless/renderer/render_debug_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

RenderDebugPass::RenderDebugPass(Renderer& renderer)
    : RendererPass (renderer)
    , helper {renderer.getSettings()} {
}

void RenderDebugPass::render(
        [[maybe_unused]] InstanceRenderer &renderer,
        Scene &scene,
        Context &ctx,
        const Assets &assets,
        const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {
    helper.render(ctx, assets, camera, scene.getLighting(), scene);
}