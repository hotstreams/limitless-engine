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

#include "limitless/core/profiler.hpp"

#include <algorithm>

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
        [[maybe_unused]] UniformSetter &setter) {
    ProfilerScope profile_scope {"SSAOPass"};

    ssao.draw(ctx, assets, renderer.getPass<DeferredFramebufferPass>().getDepth(), renderer.getPass<DeferredFramebufferPass>().getNormal());

    ctx.setViewPort(renderer.getResolution());
}

void SSAOPass::onFramebufferChange(glm::uvec2 size) {
    ssao.onFramebufferChange(size);
}

void SSAOPass::update([[maybe_unused]] Scene &scene, const Camera &camera) {
    ssao.update(camera, renderer.getSettings().ssao_settings);
}

void SSAOPass::addUniformSetter(UniformSetter &setter) {
    setter.add([&](ShaderProgram& shader){
        shader.setUniform("_ssao_texture", getResult());
        const auto& rs = renderer.getSettings();
        float edge = 0.f;
        if (rs.ssao_high_quality_deferred_upsampling) {
            edge = 1.0f / std::max(rs.ssao_deferred_upsample_bilateral_threshold, 1e-6f);
        }
        shader.setUniform("_ssao_sampling_quality_edge", edge);
    });
}

