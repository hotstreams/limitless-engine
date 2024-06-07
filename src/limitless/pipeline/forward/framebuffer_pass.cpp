#include <limitless/pipeline/forward/framebuffer_pass.hpp>

#include <limitless/core/texture/texture_builder.hpp>

using namespace Limitless;

FramebufferPass::FramebufferPass(Pipeline& pipeline, RenderTarget& _target)
    : PipelinePass(pipeline)
    , target {_target} {
}

FramebufferPass::FramebufferPass(Pipeline& pipeline, Context& ctx)
    : PipelinePass(pipeline)
    , target {framebuffer} {

    auto color = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGBA16F)
            .size(ctx.getSize())
            .format(Texture::Format::RGBA)
            .data_type(Texture::DataType::Float)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    auto depth = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::Depth32F)
            .size(ctx.getSize())
            .format(Texture::Format::DepthComponent)
            .data_type(Texture::DataType::Float)
            .min_filter(Texture::Filter::Linear)
            .mag_filter(Texture::Filter::Linear)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, color}
                << TextureAttachment{FramebufferAttachment::Depth, depth};

    framebuffer.drawBuffer(FramebufferAttachment::Color0);
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void FramebufferPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.setViewPort(ctx.getSize());
    target.clear();
}