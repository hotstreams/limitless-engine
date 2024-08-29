#include <limitless/renderer/deferred_framebuffer_pass.hpp>

#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/core/texture/texture_builder.hpp>

using namespace Limitless;

DeferredFramebufferPass::DeferredFramebufferPass(Renderer& renderer)
    : RendererPass(renderer)
    , framebuffer {} {
    //TODO: revisit format
    // R11G11B10?
    auto albedo = Texture::Builder::asRGB16NearestClampToEdge(renderer.getResolution());
    auto normal = Texture::Builder::asRGB16SNORMNearestClampToEdge(renderer.getResolution());
    auto props = Texture::Builder::asRGB16NearestClampToEdge(renderer.getResolution());
    auto emissive = Texture::Builder::asRGB16FNearestClampToEdge(renderer.getResolution());
    auto info = Texture::Builder::asRGB16NearestClampToEdge(renderer.getResolution());
    auto outline = Texture::Builder::asRGBA16NearestClampToEdge(renderer.getResolution());

    auto depth = Texture::Builder::asDepth32F(renderer.getResolution());

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, albedo}
                << TextureAttachment{FramebufferAttachment::Color1, normal}
                << TextureAttachment{FramebufferAttachment::Color2, props}
                << TextureAttachment{FramebufferAttachment::Color3, emissive}
                << TextureAttachment{FramebufferAttachment::Color4, info}
                << TextureAttachment{FramebufferAttachment::Color5, outline}
                << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void DeferredFramebufferPass::render(
        [[maybe_unused]] InstanceRenderer &renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        [[maybe_unused]] const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {
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
        FramebufferAttachment::Color4,
        FramebufferAttachment::Color5
    });

    framebuffer.clear();
}

void DeferredFramebufferPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
