#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/ssao_pass.hpp>
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
#include <limitless/camera.hpp>
#include <limitless/renderer/gbuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>

using namespace Limitless;

SSAOPass::SSAOPass(Renderer& renderer)
    : RendererPass {renderer}
    , ssao {renderer} {
}

void SSAOPass::render(
        [[maybe_unused]] InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        UniformSetter &setter) {
    ProfilerScope profile_scope {"SSAOPass"};

    ssao.draw(ctx, assets, renderer.getPass<DeferredFramebufferPass>().getDepth());
}

void SSAOPass::onFramebufferChange(glm::uvec2 size) {
    ssao.onFramebufferChange(size);
}

void SSAOPass::update(Scene &scene, const Camera &camera) {
    ssao.update(camera);
}

void SSAOPass::addUniformSetter(UniformSetter &setter) {
    setter.add([&](ShaderProgram& shader){
        shader.setUniform("_ssao_texture", getResult());
    });
}

