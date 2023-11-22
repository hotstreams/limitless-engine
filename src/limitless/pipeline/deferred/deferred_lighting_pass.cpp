#include <limitless/pipeline/deferred/deferred_lighting_pass.hpp>

#include <limitless/assets.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform_setter.hpp"
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/common/ssao_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>

using namespace Limitless;

DeferredLightingPass::DeferredLightingPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : PipelinePass(pipeline)
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdge(frame_size)} {
}

std::shared_ptr<Texture> DeferredLightingPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void DeferredLightingPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    framebuffer.clear();

    auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

    auto& shader = assets.shaders.get("deferred");

    shader .setUniform("_base_texture", gbuffer.getAlbedo())
           .setUniform("_normal_texture", gbuffer.getNormal())
           .setUniform("_props_texture", gbuffer.getProperties())
           .setUniform("_depth_texture", gbuffer.getDepth())
           .setUniform("_emissive_texture", gbuffer.getEmissive());

    setter(shader);

    shader.use();

    assets.meshes.at("quad")->draw();
}

void DeferredLightingPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
