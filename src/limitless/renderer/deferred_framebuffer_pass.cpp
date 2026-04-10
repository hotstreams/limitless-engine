#include <limitless/renderer/deferred_framebuffer_pass.hpp>

#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/profiler.hpp>

using namespace Limitless;

DeferredFramebufferPass::DeferredFramebufferPass(Renderer& renderer)
    : RendererPass(renderer)
    , framebuffer {} {
    // Low-end friendly G-Buffer formats to reduce bandwidth:
    // - Albedo: RGB8 (linear)
    // - Normal: RGB8_SNORM (keeps compatibility with existing vec3 writes)
    // - Props:  RGB8 (roughness/metal/ao)
    // - Emissive: RGB8 (sufficient for low-end; switch back to 16F if HDR needed)
    // - Info:  RGB8
    // - Outline: RGBA8
    auto albedo = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGB8)
        .format(Texture::Format::RGB)
        .data_type(Texture::DataType::UnsignedByte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

    auto normal = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGB8_SNORM)
        .format(Texture::Format::RGB)
        .data_type(Texture::DataType::Byte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

    auto props = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGB8)
        .format(Texture::Format::RGB)
        .data_type(Texture::DataType::UnsignedByte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

    auto emissive = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGB8)
        .format(Texture::Format::RGB)
        .data_type(Texture::DataType::UnsignedByte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

    auto info = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGB8)
        .format(Texture::Format::RGB)
        .data_type(Texture::DataType::UnsignedByte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

    auto outline = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGBA8)
        .format(Texture::Format::RGBA)
        .data_type(Texture::DataType::UnsignedByte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

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
    ProfilerScope scope{"DeferredFramebufferPass"};
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
