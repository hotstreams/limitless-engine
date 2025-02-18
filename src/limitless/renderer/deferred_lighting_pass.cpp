#include <limitless/renderer/deferred_lighting_pass.hpp>

#include <limitless/assets.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform_setter.hpp"
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/ssao_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

using namespace Limitless;

DeferredLightingPass::DeferredLightingPass(Renderer& renderer)
    : RendererPass {renderer}
    , framebuffer {Framebuffer::asRGB16FNearestClampToEdge(renderer.getResolution())} {
}

std::shared_ptr<Texture> DeferredLightingPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void DeferredLightingPass::render(
        [[maybe_unused]] InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene&scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        UniformSetter &setter) {
    GPUProfileScope profile_scope {global_gpu_profiler, "DeferredLightingPass"};

    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    framebuffer.clear();

    auto& gbuffer = renderer.getPass<DeferredFramebufferPass>();

    auto& shader = assets.shaders.get("deferred");

    shader .setUniform("_base_texture", gbuffer.getAlbedo())
           .setUniform("_normal_texture", gbuffer.getNormal())
           .setUniform("_props_texture", gbuffer.getProperties())
           .setUniform("_info_texture", gbuffer.getInfo())
           .setUniform("_depth_texture", gbuffer.getDepth())
           .setUniform("_emissive_texture", gbuffer.getEmissive());

    setter(shader);

    shader.use();

    assets.meshes.at("quad")->draw();
}

void DeferredLightingPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
