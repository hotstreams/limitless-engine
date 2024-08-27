#include <limitless/renderer/screen_pass.hpp>

#include <limitless/core/context.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/renderer/composite_pass.hpp>
#include <limitless/renderer/deferred_lighting_pass.hpp>
#include <limitless/renderer/renderer.hpp>

using namespace Limitless;

ScreenPass::ScreenPass(Renderer& renderer)
    : RendererPass {renderer}
    , target {&default_framebuffer} {
}

ScreenPass::ScreenPass(Renderer& renderer, RenderTarget& _target)
	: RendererPass {renderer}
	, target {&_target} {
}

void ScreenPass::render(
        [[maybe_unused]] InstanceRenderer &instance_renderer,
        [[maybe_unused]] Scene &scene, Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {

    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
	    target->clear();
        auto& shader = assets.shaders.get("quad");

        shader.setUniform("screen_texture", renderer.getPass<CompositePass>().getResult());

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void ScreenPass::setTarget(RenderTarget& _target) {
    target = &_target;
}
