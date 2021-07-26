#include <limitless/pipeline/composite_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <limitless/core/texture_builder.hpp>

using namespace Limitless;

CompositePass::CompositePass(Pipeline& pipeline, ContextEventObserver& ctx)
    : RenderPass(pipeline)
    , framebuffer {ctx} {
    TextureBuilder builder;
    auto result = builder   .setTarget(Texture::Type::Tex2D)
                            .setInternalFormat(Texture::InternalFormat::RGB8)
                            .setSize(ctx.getSize())
                            .setFormat(Texture::Format::RGB)
                            .setDataType(Texture::DataType::UnsignedByte)
                            .setMinFilter(Texture::Filter::Linear)
                            .setMagFilter(Texture::Filter::Linear)
                            .setWrapS(Texture::Wrap::ClampToEdge)
                            .setWrapT(Texture::Wrap::ClampToEdge)
                            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, result};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void CompositePass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        ctx.setViewPort(ctx.getSize());

        framebuffer.clear();
        auto& shader = assets.shaders.get("composite");

        shader << UniformSampler{"lightened", pipeline.get<DeferredFramebufferPass>().getShaded()};
        shader << UniformSampler{"emissive", pipeline.get<BlurPass>().getBlurResult()};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}
