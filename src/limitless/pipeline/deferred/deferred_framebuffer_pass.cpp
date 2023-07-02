#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>

#include <limitless/core/texture/texture_builder.hpp>

using namespace Limitless;

DeferredFramebufferPass::DeferredFramebufferPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : RenderPass(pipeline)
    , framebuffer {} {
    auto albedo = TextureBuilder::asRGBA16NearestClampToEdge(frame_size);
    auto normal = TextureBuilder::asRGB16SNORMNearestClampToEdge(frame_size);
    auto props = TextureBuilder::asRGB16NearestClampToEdge(frame_size);
    auto emissive = TextureBuilder::asRGB16FNearestClampToEdge(frame_size);
    auto depth = TextureBuilder::asDepth32F(frame_size);

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, albedo}
                << TextureAttachment{FramebufferAttachment::Color1, normal}
                << TextureAttachment{FramebufferAttachment::Color2, props}
                << TextureAttachment{FramebufferAttachment::Color3, emissive}
                << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void DeferredFramebufferPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
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
        FramebufferAttachment::Color3
    });

    framebuffer.clear();
}

void DeferredFramebufferPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
