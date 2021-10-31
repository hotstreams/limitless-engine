#include <limitless/pipeline/dof_pass.hpp>
#include <limitless/core/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/core/shader_program.hpp>

using namespace Limitless;

DoFPass::DoFPass(Pipeline& pipeline, ContextEventObserver& ctx, RenderTarget& _target)
	: RenderPass {pipeline}
	, blur {ctx}
	, target {_target} {
}

DoFPass::DoFPass(Pipeline& pipeline, ContextEventObserver& ctx)
	: RenderPass {pipeline}
	, blur {ctx}
	, target {framebuffer} {
	initialize(ctx);
}

void DoFPass::initialize(Context& ctx) {
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

void DoFPass::draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) {
	{
		blur.process(assets, getPreviousResult());
	}

	{
		target.clear();

		auto& shader = assets.shaders.get("dof");
		auto& gbuffer = pipeline.get<DeferredFramebufferPass>();

		shader << UniformSampler{ "depth_texture", gbuffer.getDepth() }
		       << UniformSampler{ "focus_texture", getPreviousResult() }
		       << UniformSampler{ "unfocus_texture", blur.getResult() }
		       << UniformValue<glm::vec2>{ "uv_focus", focus }
		       << UniformValue<glm::vec2>{ "distance", distance };

		shader.use();

		assets.meshes.at("quad")->draw();
	}
}
