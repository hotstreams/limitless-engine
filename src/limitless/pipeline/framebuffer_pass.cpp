#include <limitless/pipeline/framebuffer_pass.hpp>

#include <limitless/core/texture_builder.hpp>

using namespace Limitless;

FramebufferPass::FramebufferPass(RenderPass* prev, RenderTarget& _target)
    : RenderPass(prev)
    , target {_target} {
}

FramebufferPass::FramebufferPass(RenderPass* prev, ContextEventObserver& ctx)
    : RenderPass(prev)
    , framebuffer {ctx}
    , target {framebuffer} {
    auto param_set = [] (Texture& texture) {
        texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE};
    };

    TextureBuilder builder;
    auto color = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGBA16F)
            .setSize(ctx.getSize())
            .setFormat(Texture::Format::RGBA)
            .setDataType(Texture::DataType::Float)
            .setParameters(param_set)
            .build();

    auto depth = builder.setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::Depth32F)
            .setSize(ctx.getSize())
            .setFormat(Texture::Format::DepthComponent)
            .setDataType(Texture::DataType::Float)
            .setParameters(param_set)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, color}
                << TextureAttachment{FramebufferAttachment::Depth, depth};

    framebuffer.drawBuffer(FramebufferAttachment::Color0);
    framebuffer.checkStatus();
    framebuffer.unbind();
}

RenderTarget& FramebufferPass::getTarget() noexcept { return target; }

void FramebufferPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] const UniformSetter& setter) {
    ctx.setViewPort(ctx.getSize());
    target.clear();
}