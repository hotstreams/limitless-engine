#include <limitless/pipeline/deferred/decal_pass.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/texture/texture_builder.hpp>

using namespace Limitless;

DecalPass::DecalPass(Pipeline& pipeline)
    : PipelinePass {pipeline} {
}

void DecalPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

    setter.add([&](ShaderProgram& shader){
        shader.setUniform("depth_texture", gbuffer.getDepth());
    });

    renderer.renderDecals({ctx, assets, ShaderType::Decal, ms::Blending::Opaque, setter});
    renderer.renderDecals({ctx, assets, ShaderType::Decal, ms::Blending::Translucent, setter});
}
