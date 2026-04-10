#include <limitless/renderer/shadow_pass.hpp>

#include <limitless/scene.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/context.hpp>

#include "limitless/core/profiler.hpp"

using namespace Limitless;

DirectionalShadowPass::DirectionalShadowPass(Renderer& renderer)
    : RendererPass {renderer}
    , shadows {renderer.getSettings()} {
}

void DirectionalShadowPass::update(const RendererSettings& settings) {
    shadows.update(settings);
}

void DirectionalShadowPass::render(InstanceRenderer &renderer, Scene &scene,
                                   Context &ctx, const Assets &assets,
                                   const Camera &camera, [[maybe_unused]] UniformSetter &setter) {
    ProfilerScope profile_scope {"DirectionalShadowPass"};

    ctx.enable(Capabilities::DepthTest);
    ctx.enable(Capabilities::CullFace);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);
    ctx.setCullFace(CullFace::Front);

    // Use indirect draw for shadow casters when enabled.
    // CascadeShadowMapping owns a dedicated IndirectInstanceRenderer for shadows
    // (separate from the main-camera one) and prepares it once per frame with
    // the shadow caster list shared across all cascades.
    const bool use_indirect = this->renderer.getSettings().indirect_draw;
    shadows.draw(renderer, scene, ctx, assets, camera, use_indirect);
}

void DirectionalShadowPass::addUniformSetter(UniformSetter& setter) {
    setter.add([&] (ShaderProgram& shader) {
        shadows.setUniform(shader);
    });
}
