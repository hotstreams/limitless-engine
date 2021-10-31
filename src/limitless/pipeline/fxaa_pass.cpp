#include <limitless/pipeline/fxaa_pass.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/composite_pass.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

using namespace Limitless;

void FXAAPass::initialize(Context& ctx) {
	TextureBuilder builder;
	auto result = builder.setTarget(Texture::Type::Tex2D)
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

FXAAPass::FXAAPass(Pipeline& pipeline, Context& ctx)
    : RenderPass(pipeline)
    , target {framebuffer} {
	initialize(ctx);
}

FXAAPass::FXAAPass(Pipeline& pipeline, RenderTarget& _target)
	: RenderPass(pipeline)
	, target {_target} {
}

void FXAAPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        target.clear();
        auto& shader = assets.shaders.get("fxaa");

        shader << UniformSampler{"scene", pipeline.get<DeferredFramebufferPass>().getComposite()};

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}
