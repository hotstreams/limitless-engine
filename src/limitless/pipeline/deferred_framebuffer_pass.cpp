#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

#include <limitless/core/texture_builder.hpp>

using namespace Limitless;

DeferredFramebufferPass::DeferredFramebufferPass(Pipeline& pipeline, ContextEventObserver& ctx)
    : RenderPass(pipeline)
    , framebuffer {ctx} {
    TextureBuilder builder;
    auto albedo = builder   .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RGBA8)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

    auto normal = builder   .setTarget(Texture::Type::Tex2D)
//                            .setInternalFormat(Texture::InternalFormat::RGBA16_SNORM)
                            .setInternalFormat(Texture::InternalFormat::RGBA16F)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

    auto props = builder    .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RG8_SNORM)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

    auto emissive = builder .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RGB16F)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

	auto depth = builder.setTarget(Texture::Type::Tex2D)
						.setInternalFormat(Texture::InternalFormat::Depth32F)
						.setSize(ctx.getSize())
						.setFormat(Texture::Format::DepthComponent)
						.setDataType(Texture::DataType::Float)
						.setMinFilter(Texture::Filter::Linear)
						.setMagFilter(Texture::Filter::Linear)
						.setWrapS(Texture::Wrap::ClampToEdge)
						.setWrapT(Texture::Wrap::ClampToEdge)
						.build();

    auto shaded = builder   .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RGB8)
                            .setSize(ctx.getSize())
                            .setMinFilter(Texture::Filter::Nearest)
                            .setMagFilter(Texture::Filter::Nearest)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

	auto composite = builder    .setTarget(Texture::Type::Tex2D)
								.setInternalFormat(Texture::InternalFormat::RGB8)
								.setSize(ctx.getSize())
								.setMinFilter(Texture::Filter::Nearest)
								.setMagFilter(Texture::Filter::Nearest)
								.setWrapS(Texture::Wrap::ClampToEdge)
								.setWrapT(Texture::Wrap::ClampToEdge)
								.build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, albedo}
                << TextureAttachment{FramebufferAttachment::Color1, normal}
                << TextureAttachment{FramebufferAttachment::Color2, props}
                << TextureAttachment{FramebufferAttachment::Color3, emissive}
                << TextureAttachment{FramebufferAttachment::Color4, shaded}
                << TextureAttachment{FramebufferAttachment::Color5, composite}
                << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void DeferredFramebufferPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.setViewPort(ctx.getSize());
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