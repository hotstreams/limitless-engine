#include <limitless/renderer/shadow_pass.hpp>

#include <limitless/scene.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/renderer/renderer.hpp>

using namespace Limitless;

DirectionalShadowPass::DirectionalShadowPass(Renderer& renderer)
    : RendererPass {renderer}
    , shadows {renderer.getSettings()} {
}

void DirectionalShadowPass::render(InstanceRenderer &renderer, Scene &scene,
                                   Context &ctx, const Assets &assets,
                                   const Camera &camera, [[maybe_unused]] UniformSetter &setter) {
    shadows.draw(renderer, scene, ctx, assets, camera);
    shadows.mapData();
}

void DirectionalShadowPass::addUniformSetter(UniformSetter& setter) {
    setter.add([&] (ShaderProgram& shader) {
        shadows.setUniform(shader);
    });
}
