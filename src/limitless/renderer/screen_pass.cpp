#include <limitless/renderer/screen_pass.hpp>

#include <limitless/core/context.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/renderer/composite_pass.hpp>
#include <limitless/renderer/deferred_lighting_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/fxaa_pass.hpp>

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

        std::shared_ptr<Texture> screen;
        if (renderer.isPresent<FXAAPass>()) {
            screen = renderer.getPass<FXAAPass>().getResult();
        } else {
            screen = renderer.getPass<CompositePass>().getResult();;
        }

        shader.setUniform("screen_texture", screen);

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void ScreenPass::setTarget(RenderTarget& _target) {
    target = &_target;
}
