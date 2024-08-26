#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>

#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/instance_renderer.hpp>

using namespace Limitless;

DeferredFramebufferPass::DeferredFramebufferPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : PipelinePass(pipeline)
    , framebuffer {} {
    auto albedo = Texture::Builder::asRGB16NearestClampToEdge(frame_size);
    auto normal = Texture::Builder::asRGB16SNORMNearestClampToEdge(frame_size);
    auto props = Texture::Builder::asRGB16NearestClampToEdge(frame_size);
    auto info = Texture::Builder::asRGB16NearestClampToEdge(frame_size);
    auto emissive = Texture::Builder::asRGB16FNearestClampToEdge(frame_size);
    auto depth = Texture::Builder::asDepth32F(frame_size);

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, albedo}
                << TextureAttachment{FramebufferAttachment::Color1, normal}
                << TextureAttachment{FramebufferAttachment::Color2, props}
                << TextureAttachment{FramebufferAttachment::Color3, emissive}
                << TextureAttachment{FramebufferAttachment::Color4, info}
                << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void DeferredFramebufferPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets,
                                   const Camera &camera,
                                   UniformSetter &setter) {
    ctx.setViewPort(framebuffer.get(FramebufferAttachment::Color0).texture->getSize());
    ctx.setDepthMask(DepthMask::True);
    ctx.disable(Capabilities::Blending);
    ctx.enable(Capabilities::StencilTest);
    ctx.setStencilMask(0xFF);
	ctx.setStencilFunc(StencilFunc::Always, 1, 0xFF);
	ctx.setStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);

	framebuffer.drawBuffers({
        FramebufferAttachment::Color0,
        FramebufferAttachment::Color1,
        FramebufferAttachment::Color2,
        FramebufferAttachment::Color3,
        FramebufferAttachment::Color4
    });

    framebuffer.clear();
}

void DeferredFramebufferPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
