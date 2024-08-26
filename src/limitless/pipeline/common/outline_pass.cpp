#include <limitless/pipeline/common/outline_pass.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>

using namespace Limitless;

OutlinePass::OutlinePass(Pipeline& pipeline, glm::uvec2 size)
	: PipelinePass(pipeline) {
    auto albedo = Texture::Builder::asRGB16NearestClampToEdge(size);

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, albedo};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void
OutlinePass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
                  UniformSetter &setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    framebuffer.clear();

    auto& shader = assets.shaders.get("outline");

    shader
        .setUniform("mask_texture", pipeline.get<DeferredFramebufferPass>().getInfo())
        .setUniform("outline_color", outline_color)
        .setUniform("width", width)
        .use();

    assets.meshes.at("quad")->draw();
}

std::shared_ptr<Texture> OutlinePass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void OutlinePass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
