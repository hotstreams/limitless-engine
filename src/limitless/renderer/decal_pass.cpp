#include <limitless/renderer/decal_pass.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

DecalPass::DecalPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void DecalPass::render(
        InstanceRenderer& instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        UniformSetter &setter) {

    CpuProfileScope scope(global_profiler, "DecalPass::render");

    auto& gbuffer = renderer.getPass<DeferredFramebufferPass>();

    gbuffer.getFramebuffer().readBuffer(FramebufferAttachment::None);

    gbuffer.getFramebuffer().drawBuffers({
        FramebufferAttachment::Color0,
        FramebufferAttachment::Color1,
        FramebufferAttachment::Color2,
        FramebufferAttachment::Color3
    });

    setter.add([&](ShaderProgram& shader){
        shader.setUniform("depth_texture", gbuffer.getDepth());
        shader.setUniform("info_texture", gbuffer.getInfo());
    });

    instance_renderer.renderDecals({ctx, assets, ShaderType::Decal, ms::Blending::Opaque, setter});
    instance_renderer.renderDecals({ctx, assets, ShaderType::Decal, ms::Blending::Translucent, setter});
}
