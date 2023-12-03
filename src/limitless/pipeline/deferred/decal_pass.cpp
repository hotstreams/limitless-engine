#include <limitless/pipeline/deferred/decal_pass.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/core/texture/texture_builder.hpp>

using namespace Limitless;

DecalPass::DecalPass(Pipeline& pipeline)
    : PipelinePass {pipeline} {
    auto emissive = Texture::Builder::asRGB16FNearestClampToEdge(glm::vec2(1080, 720));

    buffer.bind();
    buffer << TextureAttachment{FramebufferAttachment::Color0, emissive};
    buffer.checkStatus();
    buffer.unbind();
}

void DecalPass::draw(Instances& instances, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    auto &gbuffer = pipeline.get<DeferredFramebufferPass>();
//buffer.bind();
//buffer.drawBuffer(FramebufferAttachment::Color0);
//buffer.readBuffer(FramebufferAttachment::None);
//    buffer.clear();
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
//
//    glFlush();
//    glFinish();

    setter.add([&](ShaderProgram& shader){
        shader.setUniform("depth_texture", gbuffer.getDepth());
    });
    for (auto& instance: instances) {
        if (instance.get().getInstanceType() == InstanceType::Decal) {
            setter.add([&] (ShaderProgram& shader) {
                shader.setUniform("decal_VP", glm::inverse(instance.get().getFinalMatrix()));
            });
            instance.get().draw(ctx, assets, ShaderType::Decal, ms::Blending::Translucent, setter);
        }
    }
}
