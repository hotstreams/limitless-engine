#include <limitless/pipeline/deferred/decal_pass.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/texture/texture_builder.hpp>

using namespace Limitless;

DecalPass::DecalPass(Pipeline& pipeline)
    : PipelinePass {pipeline} {
}

void DecalPass::draw(Instances& instances, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    auto &gbuffer = pipeline.get<DeferredFramebufferPass>();

    setter.add([&](ShaderProgram& shader){
        shader.setUniform("depth_texture", gbuffer.getDepth());
    });

    auto draw = [&](ms::Blending blending) {
        for (auto& instance: instances) {
            if (instance.get().getInstanceType() == InstanceType::Decal) {
                setter.add([&] (ShaderProgram& shader) {
                    shader.setUniform("decal_VP", glm::inverse(instance.get().getFinalMatrix()));
                });
                instance.get().draw(ctx, assets, ShaderType::Decal, blending, setter);
            }
        }
    };

    draw(ms::Blending::Opaque);
    draw(ms::Blending::Translucent);
}
