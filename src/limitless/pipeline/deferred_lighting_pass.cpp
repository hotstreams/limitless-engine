#include <limitless/pipeline/deferred_lighting_pass.hpp>

#include <limitless/assets.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform_setter.hpp"
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/ssao_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

using namespace Limitless;

DeferredLightingPass::DeferredLightingPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : RenderPass(pipeline)
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdge(frame_size)} {
}

void DeferredLightingPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    framebuffer.clear();

    auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

    auto& shader = assets.shaders.get("deferred");

    shader .setUniform("base_texture", gbuffer.getAlbedo())
           .setUniform("normal_texture", gbuffer.getNormal())
           .setUniform("props_texture", gbuffer.getProperties())
           .setUniform("depth_texture", gbuffer.getDepth())
           .setUniform("emissive_texture", gbuffer.getEmissive());

    try {
        auto& ssao = pipeline.get<SSAOPass>();
        shader.setUniform("ssao_texture", ssao.getResult());
    } catch (const pipeline_pass_not_found& e) {
        // nothing :|
    }

    setter(shader);

    shader.use();

    assets.meshes.at("quad")->draw();
}

void DeferredLightingPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}

std::shared_ptr<Texture> DeferredLightingPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}
