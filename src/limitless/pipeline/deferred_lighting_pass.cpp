#include <limitless/pipeline/deferred_lighting_pass.hpp>

#include <limitless/assets.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/pipeline/ssao_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

using namespace Limitless;

DeferredLightingPass::DeferredLightingPass(Pipeline& pipeline)
    : RenderPass(pipeline) {
}

void DeferredLightingPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    auto& gbuffer = pipeline.get<DeferredFramebufferPass>();
    auto& fb = gbuffer.getFramebuffer();

    fb.drawBuffer(FramebufferAttachment::Color4);

    auto& shader = assets.shaders.get("deferred");

    shader << UniformSampler{"albedo_texture", gbuffer.getAlbedo()}
           << UniformSampler{"normal_texture", gbuffer.getNormal()}
           << UniformSampler{"props_texture", gbuffer.getProperties()}
           << UniformSampler{"depth_texture", gbuffer.getDepth()};

    try {
        auto& ssao = pipeline.get<SSAOPass>();
        shader << UniformSampler{"ssao_texture", ssao.getResult()};
    } catch (const pipeline_pass_not_found& e) {
        // nothing :|
    }

    setter(shader);

    shader.use();

    assets.meshes.at("quad")->draw();
}
